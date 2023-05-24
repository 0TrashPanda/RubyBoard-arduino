import requests
import ctypes
import os
import argparse

# Create an ArgumentParser object
parser = argparse.ArgumentParser(description='Process a URL.')

# Add an argument for the URL with a default value
parser.add_argument('url', type=str, nargs='?', default='https://cdn.discordapp.com/attachments/646045819992145926/1109220021042954300/improtant.png',
                    help='the URL to process (default: https://cdn.discordapp.com/attachments/646045819992145926/1109220021042954300/improtant.png)')
# Parse the command-line arguments
args = parser.parse_args()
# Access the parsed URL
image_url = args.url
img_data = requests.get(image_url).content

# Save the image to a specific path
save_path = os.path.join(os.path.expanduser('~'), 'Downloads', 'important.png')
with open(save_path, 'wb') as handler:
    handler.write(img_data)

# Set the saved image as the desktop wallpaper
ctypes.windll.user32.SystemParametersInfoW(20, 0, save_path, 0)
