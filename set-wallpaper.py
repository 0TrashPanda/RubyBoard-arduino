import requests
import ctypes
import os

image_url = "https://cdn.discordapp.com/attachments/646045819992145926/1109220021042954300/improtant.png"
img_data = requests.get(image_url).content

# Save the image to a specific path
save_path = os.path.join(os.path.expanduser('~'), 'Downloads', 'important.png')
with open(save_path, 'wb') as handler:
    handler.write(img_data)

# Set the saved image as the desktop wallpaper
ctypes.windll.user32.SystemParametersInfoW(20, 0, save_path, 0)
