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

