#include <gl/glew.h>
#include <GLFW/glfw3.h> // GLFW helper library
#include <stdio.h>

// ***************************************************
// removeSquare() takes as input two points that define a square, the bottom left and the top right and the current layer
// It draws a square in the middle of the given square and then calls removeSquare() on the eight squares surrounding the middle square
void removeSquare(float bottomLeftCoordinateX, float bottomLeftCoordinateY, float topRightCoordinateX, float topRightCoordinateY, int layersDeep)
{
    // at about 8 layers deep it becomes cluttered especially since there is no zoom
    if (++layersDeep > 9)
    {
        return;
    }
    // This determines what the length of the sides are
    float xShift = topRightCoordinateX - bottomLeftCoordinateX;
    float yShift = topRightCoordinateY - bottomLeftCoordinateY;
    // This gives us the lengths of the sides of the 9 interior squares
    xShift /= 3;
    yShift /= 3;
    // everything here is represented in terms of the bottom left corner of the original square
    // this is so we can just 
    glBegin(GL_QUADS);
    glVertex2f(bottomLeftCoordinateX + xShift, bottomLeftCoordinateY + yShift); // The bottom left corner  
    glVertex2f(bottomLeftCoordinateX + xShift, bottomLeftCoordinateY + yShift * 2); // The top left corner  
    glVertex2f(bottomLeftCoordinateX + xShift * 2, bottomLeftCoordinateY + yShift * 2); // The top right corner  
    glVertex2f(bottomLeftCoordinateX + xShift * 2, bottomLeftCoordinateY + yShift); // The bottom right corner 
    glEnd();


 
    for (uint8_t j = 0; j < 3; ++j)
    {
        for (uint8_t k = 0; k < 3; ++k)
        {
            if (k == 1 && j == 1)
            {
                continue;
            }
            removeSquare(bottomLeftCoordinateX + xShift * k, bottomLeftCoordinateY + yShift * j, bottomLeftCoordinateX + xShift + xShift * k, bottomLeftCoordinateY + yShift + yShift * j, layersDeep);
        }
    }

}

int main() {
    // start GL context and O/S window using the GLFW helper library
    if (!glfwInit()) {
        fprintf(stderr, "ERROR: could not start GLFW3\n");
        return 1;
    }

    // uncomment these lines if on Apple OS X
    /*glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);*/

    GLFWwindow* window = glfwCreateWindow(640, 480, "Carpet", NULL, NULL);
    if (!window) {
        fprintf(stderr, "ERROR: could not open window with GLFW3\n");
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(window);

    // start GLEW extension handler
    glewExperimental = GL_TRUE;
    glewInit();

    // get version info
    const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
    const GLubyte* version = glGetString(GL_VERSION); // version as a string
    printf("Renderer: %s\n", renderer);
    printf("OpenGL version supported %s\n", version);

    // tell GL to only draw onto a pixel if the shape is closer to the viewer
    glEnable(GL_DEPTH_TEST); // enable depth-testing
    glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"

    /* OTHER STUFF GOES HERE NEXT */
    float points[] = {
    0.0f,  0.0f,  0.0f,
    0.0f, -0.5,  0.0f,
    0.5, -0.5,  0.0f,
    0.0f,  0.0f,  0.0f,
    0.5, 0,  0.0f,
    0.5, -0.5,  0.0f
    };

    GLuint vbo = 0;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 18 * sizeof(float), points, GL_STATIC_DRAW);

    GLuint vao = 0;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    const char* vertex_shader =
        "#version 400\n"
        "in vec3 vp;"
        "void main() {"
        "  gl_Position = vec4(vp, 1.0);"
        "}";

    const char* fragment_shader =
        "#version 400\n"
        "out vec4 frag_colour;"
        "void main() {"
        "  frag_colour = vec3(1.0, 1.0, 1.0);"
        "}";

    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vertex_shader, NULL);
    glCompileShader(vs);
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragment_shader, NULL);
    glCompileShader(fs);

    GLuint shader_programme = glCreateProgram();
    glAttachShader(shader_programme, fs);
    glAttachShader(shader_programme, vs);
    glLinkProgram(shader_programme);

    glEnable(GL_POINT_SMOOTH);

    while (!glfwWindowShouldClose(window)) {
        // wipe the drawing surface clear
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(shader_programme);
        glBindVertexArray(vao);
        // draw points 0-3 from the currently bound VAO with current in-use shader
        //glDrawArrays(GL_TRIANGLES, 0, 9);
        //glDrawArrays(GL_TRIANGLES, 10, 18);
        //glBegin(GL_QUADS);
        //glVertex2f(-1.0f, -1.0f); // The bottom left corner  
        //glVertex2f(-1.0f, 1.0f); // The top left corner  
        //glVertex2f(1.0f, 1.0f); // The top right corner  
        //glVertex2f(1.0f, -1.0f); // The bottom right corner 
        //glEnd();
        // update other events like input handling 
        removeSquare(0, 0, 1, 1, 0);
        glfwPollEvents();
        // put the stuff we've been drawing onto the display
        glfwSwapBuffers(window);
    }

    // close GL context and any other GLFW resources
    glfwTerminate();
    return 0;
}