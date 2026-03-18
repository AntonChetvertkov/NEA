import os
os.system("pip install PIL")
# from PIL import Image
# import sys

# def png_to_bitmap(input_path, output_path, array_name="bitmap"):
#     img = Image.open(input_path).convert("RGBA")
#     img = img.resize((128, 64))

#     pixels = []
#     for y in range(64):
#         for x in range(128):
#             r, g, b, a = img.getpixel((x, y))
#             brightness = 0.299 * r + 0.587 * g + 0.114 * b
#             pixels.append(1 if (a > 128 and brightness > 128) else 0)

#     bytes_list = []
#     for i in range(0, len(pixels), 8):
#         byte = 0
#         for bit in range(8):
#             if pixels[i + bit]:
#                 byte |= (1 << (7 - bit))
#         bytes_list.append(byte)

#     with open(output_path, "w") as f:
#         f.write("#pragma once\n")
#         f.write("#include <pgmspace.h>\n\n")
#         f.write(f"const uint8_t {array_name}[] PROGMEM = {{\n  ")
#         for i, b in enumerate(bytes_list):
#             f.write(f"0x{b:02X}")
#             if i < len(bytes_list) - 1:
#                 f.write(", ")
#             if (i + 1) % 16 == 0:
#                 f.write("\n  ")
#         f.write("\n};\n")

#     print(f"Done: {len(bytes_list)} bytes written to {output_path}")

# if __name__ == "__main__":
#     if len(sys.argv) < 3:
#         print("Usage: python png_to_bitmap.py input.png output.h [array_name]")
#         sys.exit(1)
#     name = sys.argv[3] if len(sys.argv) > 3 else "bitmap"
#     png_to_bitmap(sys.argv[1], sys.argv[2], name)