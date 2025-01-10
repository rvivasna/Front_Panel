from Front_Panel import AdafruitGfxRemote
import time

# import syslog

#285 3496 308 3615 4

ILI9341_BLACK = 0x0000
ILI9341_NAVY = 0x000F
ILI9341_DARKGREEN = 0x03E0
ILI9341_DARKCYAN = 0x03EF
ILI9341_MAROON = 0x7800
ILI9341_PURPLE = 0x780F
ILI9341_OLIVE = 0x7BE0
ILI9341_LIGHTGREY = 0xC618
ILI9341_DARKGREY = 0x7BEF
ILI9341_BLUE = 0x001F
ILI9341_GREEN = 0x07E0
ILI9341_CYAN = 0x07FF
ILI9341_RED = 0xF800
ILI9341_MAGENTA = 0xF81F
ILI9341_YELLOW = 0xFFE0
ILI9341_WHITE = 0xFFFF
ILI9341_ORANGE = 0xFD20
ILI9341_GREENYELLOW = 0xAFE5
ILI9341_PINK = 0xFC18

if __name__ == '__main__':
    tft = AdafruitGfxRemote('192.168.1.14', 23, 480, 320)
    # tft.change_port(1456)
    tft.fill_screen(ILI9341_BLACK)
    tft.set_text_color(ILI9341_YELLOW)
    tft.set_text_size(3)
    tft.set_text_wrap()
    tft.set_cursor(10, 10)
    tft.fill_rect(0, 0, 320, 20, ILI9341_RED)
    tft.stack_color()
    tft.print(ILI9341_BLACK,ILI9341_PINK,'Hola  X')
    for cx in range(10,35):
        tft.set_cursor(cx, 30)
        tft.print(ILI9341_BLACK, ILI9341_PINK, 'Hola   Y')
    # tft.render()
    tft.unstack_color()
    tft.fill_rect(0, 120, 320, 20, ILI9341_BLUE)
    tft.draw_line(10,100,90,100,ILI9341_BLUE)
    tft.set_text_wrap()
    # tft.progress_bar(10,100,200,10,ILI9341_RED, ILI9341_BLACK, 10, 5, 1)
    # tft.progress_bar(10, 100, 160, 10, ILI9341_YELLOW, ILI9341_BLACK, 10, 5, 1)
    # tft.progress_bar(10, 100, 50, 10, ILI9341_GREEN, ILI9341_BLACK, 10, 5, 1)
    tft.render()
    #tft.change_port(23)
    #time.sleep(0.5)

# Cual es el siguiente paso? Por muy deprisa que ejecute las instrucciones siempre se puede producir que se pierdan ordenes enviadas por
# que el ESP32 esta ejecutando instrucciones y no esta escuchando. Hay que habilitar un protocolo go/nogo