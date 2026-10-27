import os
import shutil
import subprocess
import sys
import re


def separate_audio(input_file):

    # ==========================================================
    # Locate OfforAI root directory
    #
    # OfforAI/
    # ├── demucs-engine/
    # │   └── separator.py
    # ├── demucs-env/
    # ├── separated/
    # └── output/
    # ==========================================================

    engine_dir = os.path.dirname(
        os.path.abspath(__file__)
    )

    offor_ai_dir = os.path.dirname(
        engine_dir
    )

    # ==========================================================
    # Input file
    # ==========================================================

    input_file = os.path.abspath(
        input_file
    )

    if not os.path.isfile(input_file):

        print(
            f"ERROR: Input file not found:\n{input_file}",
            flush=True
        )

        sys.exit(1)

    # ==========================================================
    # USER DATA DIRECTORIES
    # ==========================================================

    user_profile = os.environ.get(
        "USERPROFILE",
        os.path.expanduser("~")
    )

    offor_documents_dir = os.path.join(
        user_profile,
        "Documents",
        "Offor Stem Splitter"
    )

    output_dir = os.path.join(
        offor_documents_dir,
        "Exports"
    )

    separated_dir = os.path.join(
        offor_documents_dir,
        "Separated"
    )

    os.makedirs(
        output_dir,
        exist_ok=True
    )

    print(
        f"OFFOR_DEBUG: Created/checking output directory: {output_dir}",
        flush=True
    )

    print(
        f"OFFOR_DEBUG: Output directory exists: {os.path.isdir(output_dir)}",
        flush=True
    )

    os.makedirs(
        separated_dir,
        exist_ok=True
    )

    print(
        f"OFFOR_DEBUG: Created/checking separated directory: {separated_dir}",
        flush=True
    )

    print(
        f"OFFOR_DEBUG: Separated directory exists: {os.path.isdir(separated_dir)}",
        flush=True
    )

    # ==========================================================
    # Startup messages
    # ==========================================================

    print(
        "Starting Offor AI Stem Separation...",
        flush=True
    )

    print(
        f"Input: {input_file}",
        flush=True
    )

    print(
        f"Separated directory: {separated_dir}",
        flush=True
    )

    print(
        f"Output directory: {output_dir}",
        flush=True
    )

    # ==========================================================
    progress_file = os.path.abspath(sys.argv[3]) if len(sys.argv) > 3 else None

    def publish_progress(progress):
        if not progress_file:
            return
        temporary_file = progress_file + ".tmp"
        try:
            with open(temporary_file, "w", encoding="ascii") as progress_stream:
                progress_stream.write(str(int(progress)))
            os.replace(temporary_file, progress_file)
        except OSError:
            try:
                with open(progress_file, "w", encoding="ascii") as progress_stream:
                    progress_stream.write(str(int(progress)))
            except OSError:
                pass

    # Initial progress
    # ==========================================================

    publish_progress(0)

    print(
        "OFFOR_PROGRESS:0",
        flush=True
    )

    # ==========================================================
    # Run Demucs
    # ==========================================================

    model_name = "htdemucs_6s"
    if len(sys.argv) > 2 and sys.argv[2] == "htdemucs":
        model_name = "htdemucs"

    command = [
        sys.executable,
        "-u",
        "-m",
        "demucs",
        "-n",
        model_name,
        "-o",
        separated_dir,
        input_file
    ]

    print(
        "\nRunning Demucs...\n",
        flush=True
    )

    # ==========================================================
    # Start Demucs
    #
    # stderr is redirected into stdout because tqdm normally
    # writes its progress information to stderr.
    # ==========================================================

    try:

        process = subprocess.Popen(
            command,
            cwd=engine_dir,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            text=True,
            bufsize=1
        )

    except Exception as error:

        print(
            f"ERROR: Could not start Demucs: {error}",
            flush=True
        )

        sys.exit(1)

    last_progress = -1

    # ==========================================================
    # Read Demucs output
    #
    # IMPORTANT:
    #
    # tqdm can use carriage returns (\r) instead of normal
    # newline characters (\n).
    #
    # Therefore we process both.
    # ==========================================================

    try:

        while True:

            char = process.stdout.read(1)

            if char == "":
                break

            # --------------------------------------------------
            # Keep a small line buffer.
            # --------------------------------------------------

            if "line_buffer" not in locals():
                line_buffer = ""

            # --------------------------------------------------
            # tqdm uses \r to redraw the same line.
            # Treat it as a complete output line.
            # --------------------------------------------------

            if char == "\r" or char == "\n":

                if not line_buffer:
                    continue

                line = line_buffer

                line_buffer = ""

                # ==============================================
                # Print original Demucs output
                # ==============================================

                print(
                    line,
                    flush=True
                )

                # ==============================================
                # Find percentage
                # ==============================================

                matches = re.findall(
                    r'(\d+(?:\.\d+)?)%',
                    line
                )

                if matches:

                    try:

                        progress = float(
                            matches[-1]
                        )

                        progress = max(
                            0.0,
                            min(
                                100.0,
                                progress
                            )
                        )

                        progress_int = int(
                            round(progress)
                        )

                        # ======================================
                        # Only send when percentage changes.
                        # ======================================

                        if progress_int != last_progress:

                            publish_progress(progress_int)
                            print(
                                f"OFFOR_PROGRESS:{progress_int}",
                                flush=True
                            )

                            last_progress = (
                                progress_int
                            )

                    except ValueError:
                        pass

            else:

                line_buffer += char

                # ------------------------------------------------
                # Safety limit.
                # ------------------------------------------------

                if len(line_buffer) > 10000:

                    line_buffer = (
                        line_buffer[-5000:]
                    )

    except Exception as error:

        print(
            f"ERROR while reading Demucs output: {error}",
            flush=True
        )

        process.kill()

        process.wait()

        sys.exit(1)

    # ==========================================================
    # Process remaining output
    # ==========================================================

    if "line_buffer" in locals() and line_buffer:

        line = line_buffer

        print(
            line,
            flush=True
        )

        matches = re.findall(
            r'(\d+(?:\.\d+)?)%',
            line
        )

        if matches:

            try:

                progress = float(
                    matches[-1]
                )

                progress = max(
                    0.0,
                    min(
                        100.0,
                        progress
                    )
                )

                progress_int = int(
                    round(progress)
                )

                if progress_int != last_progress:

                    publish_progress(progress_int)
                    print(
                        f"OFFOR_PROGRESS:{progress_int}",
                        flush=True
                    )

                    last_progress = progress_int

            except ValueError:
                pass

    # ==========================================================
    # Wait for Demucs
    # ==========================================================

    return_code = process.wait()

    if return_code != 0:

        print(
            "\nDemucs failed.",
            flush=True
        )

        print(
            f"Exit code: {return_code}",
            flush=True
        )

        sys.exit(
            return_code
        )

    # ==========================================================
    # Force final progress
    # ==========================================================

    if last_progress != 100:

        publish_progress(100)

        print(
            "OFFOR_PROGRESS:100",
            flush=True
        )

    # ==========================================================
    # Determine song name
    # ==========================================================

    filename = os.path.splitext(
        os.path.basename(input_file)
    )[0]

    # ==========================================================
    # Find Demucs output
    # ==========================================================

    demucs_folder = os.path.join(
        separated_dir,
        model_name,
        filename
    )

    if not os.path.isdir(demucs_folder):

        print(
            "\nERROR: Demucs output folder was not found:",
            flush=True
        )

        print(
            demucs_folder,
            flush=True
        )

        sys.exit(1)

    # ==========================================================
    # Create Offor output folder
    # ==========================================================

    offor_folder = os.path.join(
        output_dir,
        filename
    )

    os.makedirs(
        offor_folder,
        exist_ok=True
    )

    # ==========================================================
    # Rename Demucs stems
    # ==========================================================

    rename = {

        "vocals.wav":
            "01_Vocals.wav",

        "drums.wav":
            "02_Drums.wav",

        "bass.wav":
            "03_Bass.wav",

        "other.wav":
            "04_Instrumental.wav"
    }

    if model_name == "htdemucs_6s":
        rename.update({
            "guitar.wav": "05_Guitar.wav",
            "piano.wav": "06_Piano.wav"
        })

    # ==========================================================
    # Copy stems
    # ==========================================================

    for old, new in rename.items():

        source = os.path.join(
            demucs_folder,
            old
        )

        destination = os.path.join(
            offor_folder,
            new
        )

        if os.path.isfile(source):

            print(
                f"Creating {new}...",
                flush=True
            )

            shutil.copy2(
                source,
                destination
            )

        else:

            print(
                f"WARNING: Missing stem: {old}",
                flush=True
            )

    if model_name == "htdemucs":
        for extra_stem in ("05_Guitar.wav", "06_Piano.wav"):
            stale_file = os.path.join(offor_folder, extra_stem)
            if os.path.isfile(stale_file):
                os.remove(stale_file)

    # ==========================================================
    # Finished
    # ==========================================================

    print(
        "OFFOR_STEMS_READY",
        flush=True
    )

    print(
        f"OFFOR_OUTPUT:{offor_folder}",
        flush=True
    )


# ==============================================================
# MAIN
# ==============================================================

if __name__ == "__main__":

    if len(sys.argv) < 2:

        print(
            "Usage: python separator.py song.mp3",
            flush=True
        )

        sys.exit(1)

    separate_audio(
        sys.argv[1]
    )
