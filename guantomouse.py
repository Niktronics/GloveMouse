# import asyncio
# import websockets
# import vgamepad as vg

# gamepad = vg.VX360Gamepad()

# async def handler():
#     uri = "ws://192.168.1.22"
#     async with websockets.connect(uri) as websocket:
#         async for message in websocket:
#             try:
#                 if message == "LP":
#                     gamepad.press_button(vg.XUSB_BUTTON.XUSB_GAMEPAD_LEFT_SHOULDER)
#                 elif message == "LR":
#                     gamepad.release_button(vg.XUSB_BUTTON.XUSB_GAMEPAD_LEFT_SHOULDER)
#                 elif message == "RP":
#                     gamepad.press_button(vg.XUSB_BUTTON.XUSB_GAMEPAD_RIGHT_SHOULDER)
#                 elif message == "RR":
#                     gamepad.release_button(vg.XUSB_BUTTON.XUSB_GAMEPAD_RIGHT_SHOULDER)
#                 else:
#                     mx, my = map(int, message.split(","))
#                     gx = mx * 32767 // 10
#                     gy = my * 32767 // 10
#                     gamepad.left_joystick(x_value=gx, y_value=-gy)
#                     print("Joystick:", gx, gy)

#                 gamepad.update()
#             except Exception as e:
#                 print("Errore parsing:", e)

# asyncio.run(handler())


import asyncio
import websockets
import pydirectinput
pydirectinput.PAUSE = 0
pydirectinput.FAILSAFE = False

async def handler():
    uri = "ws://10.21.45.227"
    async with websockets.connect(uri) as websocket:
        async for message in websocket:
            try:
                if message == "LP":
                    pydirectinput.mouseDown(button='left')
                elif message == "LR":
                    pydirectinput.mouseUp(button='left')
                elif message == "RP":
                    pydirectinput.mouseDown(button='right')
                elif message == "RR":
                    pydirectinput.mouseUp(button='right')
                else:
                    mx, my = map(int, message.split(","))
                    pydirectinput.move(mx, -my, relative=True)
                    print("Mouse:", mx, -my)
            except Exception as e:
                print("Errore parsing:", e)

asyncio.run(handler())
