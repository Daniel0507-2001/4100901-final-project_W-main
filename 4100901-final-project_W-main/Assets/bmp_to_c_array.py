from PIL import Image

def bmp_to_c_array(filename, width, height, reverse_bits=False):
    # Load the image and convert it to monochrome
    image = Image.open(filename + ".bmp").convert('1')

    # Ensure the image has the correct dimensions
    if image.size != (width, height):
        raise ValueError(f"Image size must be {width}x{height} pixels")

    # Create a binary array to store pixel data
    bitmap = []
    for y in range(height):
        for x in range(0, width, 8):
            byte = 0
            for bit in range(8):
                if x + bit < width:
                    pixel = 1 if image.getpixel((x + bit, y)) > 0 else 0
                    if reverse_bits:
                        byte |= pixel << (7 - bit)
                    else:
                        byte |= pixel << bit
            bitmap.append(byte)

    # Write the C array to a file
    with open(filename + ".c", 'w') as file:
        file.write("#include <stdint.h>\n\n")
        file.write("const uint8_t " + filename + "[] = {\n")
        for byte in bitmap:
            file.write(f'0x{byte:02X}, ')
        file.write('\n};\n')

# Example usage for a 30x30 image
bmp_to_c_array('locked', 30, 30, reverse_bits=True)
