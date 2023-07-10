

import pygame
from pygame.locals import*

from OpenGL.GL import*
from OpenGL.GLU import *

vertices= (
    (1, -1, -1),
    (1, 1, -1),
    (-1, 1, -1),
    (-1, -1, -1),
    (1, -1, 1),
    (1, 1, 1),
    (-1, -1, 1),
    (-1, 1, 1)
    
    )

faces = (
    (0, 1, 2, 3),
    (3, 2, 7, 6),
    (6, 7, 5, 4),
    (4, 5, 1, 0),
    (1, 5, 7, 2),
    (4, 0, 3, 6)
)

texture_coords = (
    (1, 0),
    (1, 1),
    (0, 1),
    (0, 0)
)


def cube():
    glBegin(GL_QUADS)
    for face in faces:
        for vertex, texture_coord in zip(face, texture_coords):
            glTexCoord2fv(texture_coord)
            glVertex3fv(vertices[vertex])

    glEnd()

def main():
    pygame.init()
    display = (800,600)
    pygame.display.set_mode(display, DOUBLEBUF|OPENGL)
    glMatrixMode(GL_PROJECTION)
    gluPerspective(45, (display[0]/display[1]), 0.1, 50.0)
    button_down = False
    
    
    glMatrixMode(GL_MODELVIEW)
    modelMatrix = glGetFloatv(GL_MODELVIEW_MATRIX)
    #lighting and  texturing 
    
    
    glEnable(GL_LIGHTING)
    glEnable(GL_LIGHT0)

    glLight(GL_LIGHT0, GL_POSITION,  (0, 0, 1, 0)) # directional light from the front
   
    glLightfv(GL_LIGHT0, GL_AMBIENT, (0, 0, 0, 1))
    glLightfv(GL_LIGHT0, GL_DIFFUSE, (1, 1, 1, 1))
    glLightfv(GL_LIGHT0, GL_SPECULAR, (1, 1, 1, 1))
    glMaterialf(GL_FRONT, GL_SHININESS, 100.0)

    
    
    
    
    glEnable(GL_DEPTH_TEST)
    texture_surface = pygame.image.load("texture.jpg")
    texture_data = pygame.image.tostring(texture_surface, "RGB", 1)
    glEnable(GL_TEXTURE_2D)
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT)
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT)
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST)
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST)
    glTexImage2D(GL_TEXTURE_2D, 0, 3, texture_surface.get_width(), texture_surface.get_height(),
                 0, GL_RGB, GL_UNSIGNED_BYTE, texture_data)
    
      
    
    
 
    while True:
        glPushMatrix()
        glLoadIdentity()
        
        #arrow keys to move object , drag to rotate , mouse wheel for zoom
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pygame.quit()
                quit()
            if event.type == pygame.KEYDOWN:
                if event.key == pygame.K_LEFT:
                    glTranslatef(-1,0,0)
                if event.key == pygame.K_RIGHT:
                    glTranslatef(1,0,0)
                if event.key == pygame.K_UP:
                    glTranslatef(0,1,0)
                if event.key == pygame.K_DOWN:
                    glTranslatef(0,-1,0)
            if event.type == pygame.MOUSEMOTION:
                if button_down == True:
                    glRotatef(event.rel[1],1,0,0)
                    glRotatef(event.rel[0],0,1,0)
                print(event.rel)
            if event.type ==  pygame.MOUSEBUTTONDOWN:
                if event.button ==4:
                    glTranslatef(0,0,1.0)
                if event.button ==5:
                    glTranslatef(0,0,-1.0)
            
                    
        for event in pygame.mouse.get_pressed():
            print(pygame.mouse.get_pressed())
            if pygame.mouse.get_pressed()[0]==1:
                button_down = True
            elif pygame.mouse.get_pressed()[0]==0:
                button_down = False
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT)
        glMultMatrixf(modelMatrix)
        modelMatrix = glGetFloatv(GL_MODELVIEW_MATRIX)
        
        glLoadIdentity()
        glTranslatef(0, 0, -5)
        glMultMatrixf(modelMatrix)

        cube()

        glPopMatrix()
        pygame.display.flip()
        pygame.time.wait(10)
        
        
        
main()