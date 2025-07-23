import argparse
import os
import sys


def apply_patches(file_path, output_suffix="_patched"):
    output_file_path = f"{file_path}{output_suffix}"

    print(f"Opening '{file_path}'...")
    with open(file_path, "rb") as f:
        raw_bytes = bytearray(f.read())
    print(f"File '{file_path}' opened. Size: {len(raw_bytes)} bytes.")

    patches = {}

    # License checks patch
    license_check_offset = 0x2805
    patches[license_check_offset] = 0x90
    patches[license_check_offset + 1] = 0x90
    print(f"Patching license check at {hex(license_check_offset)}.")

    # Checksum/conditional jump bypass patches
    first_jumps = [
        0x1A24,
        0x1B86,
        0x1CE6,
        0x1C15,
        0x1AC1,
        0x1E95,
        0x1F9C,
        0x21D8,
        0x22D5,
        0x23D6,
        0x2407,
        0x254C,
        0x264E,
        0x269F,
        0x26B5,
        0x26F4,
        0x27F6,
    ]

    jump_to_patch_index = 6
    if jump_to_patch_index < len(first_jumps):
        target_jump_offset = first_jumps[jump_to_patch_index]
        patches[target_jump_offset] = 0xEB
        print(f"Patching jump at {hex(target_jump_offset)}.")
    else:
        print(
            f"Warning: Jump index {jump_to_patch_index} out of bounds. No patch applied for this specific jump."
        )

    print("Applying all patches...")
    applied_count = 0
    for index, new_byte in patches.items():
        if index < len(raw_bytes):
            original_byte = raw_bytes[index]
            raw_bytes[index] = new_byte
            print(
                f"  Applied patch at {hex(index)}: 0x{original_byte:02x} -> 0x{new_byte:02x}."
            )
            applied_count += 1
        else:
            print(f"  Warning: Patch offset {hex(index)} out of file bounds. Skipping.")

    print(f"Applied {applied_count} patches.")

    print(f"Saving patched file to '{output_file_path}'...")
    with open(output_file_path, "wb") as f:
        f.write(raw_bytes)
    print(f"Patched file saved as '{output_file_path}'.")


def main():
    parser = argparse.ArgumentParser(epilog="Example usage: python patch_binary.py ")
    parser.add_argument(
        "file_path",
        metavar="BINARY_FILE",
        type=str,
        help="Path to the binary file to be patched.",
    )
    parser.add_argument(
        "-o",
        "--output-suffix",
        dest="output_suffix",
        type=str,
        default="_patched",
        help="Suffix for the patched output file (default: _patched).",
    )

    args = parser.parse_args()

    if not os.path.exists(args.file_path):
        print(f"Error: Input file '{args.file_path}' does not exist.")
        parser.print_help()
        sys.exit(1)

    apply_patches(args.file_path, args.output_suffix)
    print("\nPatching process completed.")


if __name__ == "__main__":
    main()
