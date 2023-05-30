import pygame
import numpy as np

pygame.init()

width, height = 128, 81

screen = pygame.display.set_mode((width, height))

background_color = (0, 0, 0)
screen.fill(background_color)

amplitude = 150
frequency = 100
timeseps=200

start_x = width // 2
start_y = height // 2

running = True
while running:
    for step in range(timeseps):
        screen.fill(background_color)
        for x in range(width):
            for y in range(height):
                distance = np.sqrt((x - start_x)**2 + (y - start_y)**2)
                displacement = amplitude * np.sin((distance + 190 - step) / frequency)
                intensity = max(0, min((displacement-146) *36, 255))
                color = (intensity, 0, intensity)
                screen.set_at((x, y), color)
        pygame.display.flip()
pygame.quit()

