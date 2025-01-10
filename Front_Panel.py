import socket
from time import sleep


class AdafruitGfxRemote:
    __DRAW_LINE = 48
    __DRAW_FAST_VLINE = 49
    __DRAW_FAST_HLINE = 50
    __DRAW_RECT = 51
    __FILL_RECT = 52
    __FILL_SCREEN = 53
    __INVERT_DISPLAY = 54
    __WRITE_PIXEL = 55
    __SET_ROTATION = 56
    __PRINT = 57
    __SET_CURSOR = 58
    __SET_TEXT_COLOR = 59
    __SET_TEXT_SIZE = 60
    __SET_TEXT_WRAP = 61

    __DRAW_PIXEL = 62
    __DRAW_CIRCLE = 63
    __FILL_CIRCLE = 64
    __DRAW_ROUND_RECT = 65
    __FILL_ROUND_RECT = 66
    __DRAW_TRIANGLE = 67
    __FILL_TRIANGLE = 68
    __CHANGE_PORT = 69
    __PROGRES_BAR = 70
    __EXTRA_DATA = 71

    __tft_width = 0
    __tft_height = 0

    __INT_REGISTERS = 7
    __MAX_COMMAND_LEN = 255
    __MAX_COMMANDS_IN_BUFFER = 250
    __BUFFER_SIZE = ((__MAX_COMMANDS_IN_BUFFER * __MAX_COMMAND_LEN) + 1)
    __color_stack = []
    __actual_fg_color = 255
    __actual_bk_color = 0

    ipAddr = '127.0.0.1'
    num_port = 23

    def render(self):
        if len(self.command_buffer) > 0:
            self.command_buffer = 'DEBX' + self.command_buffer + '\0'

            tcp_port = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            tcp_port.connect((self.ipAddr, self.num_port))
            tcp_port.sendall(bytes(self.command_buffer.encode()))
            # except serial.SerialException as e:
            #     print('Error writing on tcp port:{}:{} {} '.format(self.ipAddr, self.num_port, e.strerror))
            #     exit_loop = True
            tcp_port.close()

            self.command_buffer = ''
            sleep(0.3)

    def __send_command(self, data_type, data_values):
        command = chr(data_type)
        if data_type == self.__PRINT:
            command = command + data_values[0] + '\0'
        else:
            for x in range(0, self.__INT_REGISTERS):
                command = command + '{:04x}'.format(data_values[x]).upper()
        # If next command will cause overflow in command buffer then force render buffer before
        if len(self.command_buffer) + len(command) > self.__BUFFER_SIZE:
            self.render()

        self.command_buffer = self.command_buffer + command

    def change_port(self, port = 23):
        self.render()
        self.__send_command(self.__CHANGE_PORT, [port, 0, 0, 0, 0, 0, 0])
        self.render()
        self.num_port = port

    def __adjust_x(self, width):
        return width if width <= self.__tft_width else self.__tft_width

    def __adjust_y(self, height):
        return height if height <= self.__tft_height else self.__tft_height

    def __adjust_width(self, x, width):
        return width if width <= x + self.__tft_width else self.__tft_width - x

    def __adjust_height(self, y, height):
        return height if height <= y + self.__tft_height else self.__tft_height - y

    def stack_color(self):
        self.__color_stack = self.__color_stack + [[self.__actual_fg_color, self.__actual_bk_color]]

    def unstack_color(self):
        if len(self.__color_stack) > 0:
            self.__actual_fg_color = self.__color_stack[len(self.__color_stack) - 1][0]
            self.__actual_bk_color = self.__color_stack[len(self.__color_stack) - 1][1]
            self.__color_stack = self.__color_stack[0:len(self.__color_stack) - 1]
            self.set_text_color(self.__actual_fg_color, self.__actual_bk_color)

    def draw_line(self, xi, yi, xe, ye, color):
        if (xi == xe):
            self.__send_command(self.__DRAW_FAST_VLINE, [xi, yi, (ye - yi) + 1, color, 0, 0, 0])
        elif (yi == ye):
            self.__send_command(self.__DRAW_FAST_HLINE, [xi, yi, (xe - xi) + 1, color, 0, 0, 0])
        else:
            self.__send_command(self.__DRAW_LINE, [xi, yi, xe, ye, color, 0, 0])

    def fill_screen(self, color):
        self.__send_command(self.__FILL_SCREEN, [color, 0, 0, 0, 0, 0, 0])

    def set_cursor(self, x, y):
        self.__send_command(self.__SET_CURSOR, [self.__adjust_x(x), self.__adjust_y(y), 0, 0, 0, 0, 0])

    def set_text_color(self, foreground=255, background=0):
        self.__send_command(self.__SET_TEXT_COLOR, [foreground, background, 0, 0, 0, 0, 0])
        self.__actual_fg_color = foreground
        self.__actual_bk_color = background

    def set_text_size(self, size=1):
        self.__send_command(self.__SET_TEXT_SIZE, [size, 0, 0, 0, 0, 0, 0])

    def set_text_wrap(self, wrap_mode=0):
        self.__send_command(self.__SET_TEXT_WRAP, [wrap_mode, 0, 0, 0, 0, 0, 0])

    def progress_bar(self, x, y, width, height, fg_color, bk_color, step, width_mark, orientation):
        self.__send_command(self.__EXTRA_DATA, [fg_color, bk_color, step, width_mark, orientation, 0, 0])
        self.__send_command(self.__PROGRES_BAR, [self.__adjust_x(x), self.__adjust_y(y), self.__adjust_width(x, width),
                                               self.__adjust_height(y, height), 0, 0, 0])

    def fill_rect(self, x, y, width, height, fill_color):
        self.__send_command(self.__FILL_RECT, [self.__adjust_x(x), self.__adjust_y(y), self.__adjust_width(x, width),
                                               self.__adjust_height(y, height), fill_color, 0, 0])

    def print(self, fg_color, bk_color, texto):
        self.set_text_color(fg_color, bk_color)
        self.__send_command(self.__PRINT, [texto, 0, 0, 0, 0, 0, 0])

    def __init__(self, ip_address, port, width, height):
        self.ipAddr = ip_address
        self.num_port = port
        self.command_buffer = ''
        self.__tft_height = height
        self.__tft_width = width
        self.set_text_color(self.__actual_fg_color, self.__actual_bk_color)
