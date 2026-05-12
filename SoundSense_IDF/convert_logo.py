from PIL import Image
import sys

def convert_to_ssd1306_array(image_path):
    img = Image.open(image_path).convert('1') # Convert to 1-bit pixel
    img = img.resize((128, 64))
    width, height = img.size
    
    # SSD1306 uses horizontal segments (8 bits per segment, vertical)
    # But wait, my driver expects [x + (y / 8) * 128]
    # This is Page Addressing Mode
    
    buffer = [0] * (width * height // 8)
    for y in range(height):
        for x in range(width):
            if img.getpixel((x, y)) > 0:
                # Page index: y // 8
                # Bit index: y % 8
                buffer[x + (y // 8) * width] |= (1 << (y % 8))
    
    return buffer

if __name__ == "__main__":
    path = r"C:\Users\emirt\.gemini\antigravity\brain\89722d50-0a40-4bbd-ac3d-37a3d18c3ba1\soundsense_oled_logo_1778513072402.png"
    arr = convert_to_ssd1306_array(path)
    print("const uint8_t soundsense_logo[] = {")
    for i in range(0, len(arr), 16):
        print("    " + ", ".join([f"0x{b:02X}" for b in arr[i:i+16]]) + ",")
    print("};")
