#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shader_m.h"
#include "camera.h"
#include "Objeto.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <ctime>
#include <random>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);

void mouse_callback(GLFWwindow *window, double xpos, double ypos);

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

void processInput(GLFWwindow *window);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
const unsigned int rows = 80;
const unsigned int cols = 60;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;
float rotacion_piramide;

glm::vec3 lightPos(1.2f, 5.0f, 5.0f);
Cubo cubo;

class TriangleRenderer {
public:
    GLint POSITION_ATTRIBUTE = 0, NORMAL_ATTRIBUTE = 1;
    bool visible = true;

    GLuint vao = 0;
    GLuint vbos[2];

    std::vector<glm::vec3> vertexData;
    std::vector<glm::vec3> normalData;

    TriangleRenderer(const std::vector<glm::vec3> &triangle) {
        setup(triangle);
    }

    ~TriangleRenderer() {
        /*glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vbos[0]);
        glDeleteBuffers(1, &vbos[1]);*/
    }

    void setup(const std::vector<glm::vec3> &triangle) {
        // Flatten vertex and normal data
        for (int i = 0; i < 3; i++) {
            vertexData.push_back(triangle[i * 2]);
            normalData.push_back(triangle[i * 2 + 1]);

        }
        if (vao == 0) {
            //GLuint vao;
            glGenVertexArrays(1, &vao);
            glBindVertexArray(vao);

            glGenBuffers(2, vbos);

            glBindBuffer(GL_ARRAY_BUFFER, vbos[0]);
            glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(vec3), vertexData.data(), GL_STATIC_DRAW);
            glVertexAttribPointer(POSITION_ATTRIBUTE, 3, GL_FLOAT, GL_FALSE, 0, (void *) 0);
            glEnableVertexAttribArray(POSITION_ATTRIBUTE);

            glBindBuffer(GL_ARRAY_BUFFER, vbos[1]);
            glBufferData(GL_ARRAY_BUFFER, normalData.size() * sizeof(vec3), normalData.data(), GL_STATIC_DRAW);
            glVertexAttribPointer(NORMAL_ATTRIBUTE, 3, GL_FLOAT, GL_TRUE, 0, (void *) 0);
            glEnableVertexAttribArray(NORMAL_ATTRIBUTE);
        }
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        //indices_size = indices.size();
    }

    void render(Shader &sh, int i) {
        glm::mat4 model = mat4(1.0);
        sh.setMat4("model", model);
        if (i % 2 == 0)
            sh.setVec3("objectColor", glm::vec3(1, 0, 0));
        else
            sh.setVec3("objectColor", glm::vec3(0, 0, 1));

        if (visible) {
            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLES, 0, 3);
            glBindVertexArray(0);
        }
    }
};

std::vector<std::vector<double>> read_matrix_from_file(const std::string &filename) {
    std::ifstream file(filename);
    std::vector<std::vector<double>> matrix;
    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        double val;
        std::vector<double> row;
        while (ss >> val) {
            row.push_back(val);
            ss.ignore();
        }
        matrix.push_back(row);
    }
    file.close();
    return matrix;
}

std::vector<std::vector<double>> random_matrix() {
    std::srand(static_cast<unsigned int>(std::time(0)));

    std::vector<std::vector<double>> randomMatrix(rows, std::vector<double>(cols));

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            randomMatrix[i][j] = std::rand() % 11 - 5;  // Adjust the range as needed
        }
    }
    return randomMatrix;
}

glm::vec3 calculateNormal(const glm::vec3 &vertex1, const glm::vec3 &vertex2, const glm::vec3 &vertex3) {
    glm::vec3 edge1 = vertex2 - vertex1;
    glm::vec3 edge2 = vertex3 - vertex1;
    glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));
    return normal;
}

int main() {
    std::vector<std::vector<glm::vec3>> triangles;
    std::vector<TriangleRenderer> todos;
    bool flag = false;
    vector<vector<double>> randomYValues;
    if (flag) randomYValues = read_matrix_from_file("../random_matrix.txt");
    else randomYValues = random_matrix();
    for (int i = 0; i < randomYValues.size() - 1; ++i) {
        for (int j = 0; j < randomYValues[0].size() - 1; ++j) {
            glm::vec3 vertex1(double(i), randomYValues[i][j], double(j));
            glm::vec3 vertex2(double(i + 1), randomYValues[i + 1][j], double(j));
            glm::vec3 vertex3(double(i), randomYValues[i][j + 1], double(j + 1));
            glm::vec3 vertex4(double(i + 1), randomYValues[i + 1][j + 1], double(j + 1));
            auto nt1 = calculateNormal(vertex3, vertex1, vertex2);
            auto nt2 = calculateNormal(vertex4, vertex3, vertex2);


            std::vector<glm::vec3> triangle1 = {vertex3, -nt1,
                                                vertex1, -nt1,
                                                vertex2, -nt1};
            std::vector<glm::vec3> triangle2 = {vertex4, -nt2,
                                                vertex3, -nt2,
                                                vertex2, -nt2};
            triangles.push_back(triangle1);
            triangles.push_back(triangle2);
        }
    }
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef _APPLE_
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    // configure global opengl state
    glEnable(GL_DEPTH_TEST);
    // build and compile our shader zprogram
    Shader lightingShader("../2.2.basic_lighting.vs", "../2.2.basic_lighting.fs");
    //Shader lightingShader("../1.basico_sin_luz.vs", "../1.basico_sin_luz.fs");
    Shader lightCubeShader("../2.2.light_cube.vs", "../2.2.light_cube.fs");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // first, configure the cube's VAO (and VBO)
    cubo.setup();
    unsigned int VBO = cubo.vbo_position;
    // second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
    unsigned int lightCubeVAO;
    glGenVertexArrays(1, &lightCubeVAO);
    glBindVertexArray(lightCubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // note that we update the lamp's position attribute's stride to reflect the updated buffer data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);

    for (const auto &triangle: triangles) {
        TriangleRenderer t1(triangle);
        todos.push_back(t1);
    }

    // render loop
    while (!glfwWindowShouldClose(window)) {
        // per-frame time logic
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        processInput(window);

        // render
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // be sure to activate shader when setting uniforms/drawing objects
        lightingShader.use();
        lightingShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
        lightingShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
        lightingShader.setVec3("lightPos", lightPos);
        lightingShader.setVec3("viewPos", camera.Position);

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float) SCR_WIDTH / (float) SCR_HEIGHT, 0.1f,
                                                100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);

        // world transformation
        glm::mat4 model = glm::mat4(1.0f);
        lightingShader.setMat4("model", model);


        for (int i = 0; i < todos.size(); i++) todos[i].render(lightingShader, i);


        // also draw the lamp object
        lightCubeShader.use();
        lightCubeShader.setMat4("projection", projection);
        lightCubeShader.setMat4("view", view);
        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
        lightCubeShader.setMat4("model", model);

        glBindVertexArray(lightCubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    glDeleteVertexArrays(1, &cubo.vao);
    glDeleteVertexArrays(1, &lightCubeVAO);
    glDeleteBuffers(1, &VBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
        rotacion_piramide += 2;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow *window, double xposIn, double yposIn) {
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}