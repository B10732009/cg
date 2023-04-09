#include <algorithm>
#include <memory>
#include <vector>
#include <cmath>
#include <iostream>

#include <GLFW/glfw3.h>
#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#undef GLAD_GL_IMPLEMENTATION
#include <glm/glm.hpp>

#include "camera.h"
#include "opengl_context.h"
#include "utils.h"

#define ANGEL_TO_RADIAN(x) (float)((x)*M_PI / 180.0f) 
#define RADIAN_TO_ANGEL(x) (float)((x)*180.0f / M_PI) 

#define CIRCLE_SEGMENT 64

/* Components size definition */
#define ARM_LEN 1.0f
#define ARM_RADIUS 0.05f
#define ARM_DIAMETER (ARM_RADIUS * 2)
#define JOINT_RADIUS 0.05f
#define JOINT_DIAMETER (JOINT_RADIUS * 2)
#define JOINT_WIDTH 0.1f
#define BASE_RADIUS 0.5f
#define BASEE_DIAMETER (BASE_RADIUS * 2)
#define BASE_HEIGHT 0.1f
#define TARGET_RADIUS 0.05f
#define TARGET_DIAMETER (TARGET_RADIUS * 2)
#define TARGET_HEIGHT 0.1f
#define ROTATE_SPEED 1.0f 
#define CATCH_POSITION_OFFSET 0.05f
#define TOLERANCE 0.1f

#define RED 0.905f, 0.298f, 0.235f
#define BLUE 0.203f, 0.596f, 0.858f
#define GREEN 0.18f, 0.8f, 0.443f
#define WHITE 1.000f, 0.921f, 0.803f

// degree of rotation of current position
float joint0_degree = 0;
float joint1_degree = 0;
float joint2_degree = 0;

// check if the joint is rotating and its direction
// -1 rotate clockwise
//  0 not rotate
//  1 rotate counterclockwise
int joint0_is_rotating = 0;
int joint1_is_rotating = 0;
int joint2_is_rotating = 0;

bool is_catching = false;   // is pressing SPACE or not
bool can_catch = false;     // the position of arm endpoint can catch or not

bool bonus = false;         // open the bonus features or not

// physical falling simulation
float velocity = 0.0f;
float acceleration = -0.001f;

glm::vec3 target_pos(1.0f, 0.05f, 1.0f);

void resizeCallback(GLFWwindow* window, int width, int height) {
  OpenGLContext::framebufferResizeCallback(window, width, height);
  auto ptr = static_cast<Camera*>(glfwGetWindowUserPointer(window));
  if (ptr) {
    ptr->updateProjectionMatrix(OpenGLContext::getAspectRatio());
  }
}

void keyCallback(GLFWwindow* window, int key, int, int action, int) {
  // There are three actions: press, release, hold(repeat)
  if (action == GLFW_REPEAT) return;
  // Press ESC to close the window.
  if (key == GLFW_KEY_ESCAPE) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
    return;
  }
  /* TODO#4-1: Detect key-events, perform rotation or catch target object
   *       1. Use switch/case to find the key you want.
   *       2. Define and modify some global variable to trigger update in rendering loop
   * Hint:
   *       glfw3's key list (https://www.glfw.org/docs/3.3/group__keys.html)
   *       glfw3's action codes (https://www.glfw.org/docs/3.3/group__input.html#gada11d965c4da13090ad336e030e4d11f)
   * Note:
   *       You should finish your robotic arm first.
   *       Otherwise you will spend a lot of time debugging this with a black screen.
   */

  switch(key)
  {
    case GLFW_KEY_U:
        joint0_is_rotating = (joint0_is_rotating + 1) % 2;
        break;
    case GLFW_KEY_J:
        joint0_is_rotating = -(joint0_is_rotating + 1) % 2;
        break;
    case GLFW_KEY_K:
        joint1_is_rotating = (joint1_is_rotating + 1) % 2;
        break;
    case GLFW_KEY_I:
        joint1_is_rotating = -(joint1_is_rotating + 1) % 2;
        break;
    case GLFW_KEY_O:
        joint2_is_rotating = -(joint2_is_rotating + 1) % 2;
        break;
    case GLFW_KEY_L:
        joint2_is_rotating = (joint2_is_rotating + 1) % 2;
        break;
    case GLFW_KEY_SPACE:
        is_catching = !is_catching;
        break;
    // bonus features 
    case GLFW_KEY_B:
        if (action == GLFW_PRESS)
            bonus = !bonus;
        break;
  }
}

void initOpenGL() {
  // Initialize OpenGL context, details are wrapped in class.
#ifdef __APPLE__
  // MacOS need explicit request legacy support
  OpenGLContext::createContext(21, GLFW_OPENGL_ANY_PROFILE);
#else
  OpenGLContext::createContext(21, GLFW_OPENGL_ANY_PROFILE);
//  OpenGLContext::createContext(43, GLFW_OPENGL_COMPAT_PROFILE);
#endif
  GLFWwindow* window = OpenGLContext::getWindow();
  /* TODO#0: Change window title to "HW1 - `your student id`"
   *         Ex. HW1 - 311550000 
   */
  glfwSetWindowTitle(window, "HW1 - 311552013");
  glfwSetKeyCallback(window, keyCallback);
  glfwSetFramebufferSizeCallback(window, resizeCallback);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
#ifndef NDEBUG
  OpenGLContext::printSystemInfo();
  // This is useful if you want to debug your OpenGL API calls.
  OpenGLContext::enableDebugCallback();
#endif
}

void drawUnitCylinder() {
  /* TODO#2-1: Render a unit cylinder
   * Hint:
   *       glBegin/glEnd (https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/glBegin.xml)
   *       glColor3f (https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/glBegin.xml)
   *       glVertex3f (https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/glVertex.xml)
   *       glNormal (https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/glNormal.xml)
   *       glScalef (https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/glScale.xml)
   * Note:
   *       You can refer to ppt "Draw Cylinder" page and `CIRCLE_SEGMENT`
   *       You should set normal for lighting
   */

    glBegin(GL_TRIANGLES);
    float slice = ANGEL_TO_RADIAN(360.0f / CIRCLE_SEGMENT);
    for (int i = 0; i < CIRCLE_SEGMENT; i++) {
        // bottom
        glNormal3f(0.0f, -1.0f, 1.0f);
        glVertex3f(0.0f, 0.0f, 0.0f);
        glVertex3f(std::sin(slice * (CIRCLE_SEGMENT - i)), 0.0f, std::cos(slice * (CIRCLE_SEGMENT - i)));
        glVertex3f(std::sin(slice * (CIRCLE_SEGMENT - (i + 1))), 0.0f, std::cos(slice * (CIRCLE_SEGMENT - (i + 1))));
        
        // top
        glNormal3f(0.0f, 1.0f, 0.0f);
        glVertex3f(0.0f, 1.0f, 0.0f);
        glVertex3f(std::sin(slice * i), 1.0f, std::cos(slice * i));
        glVertex3f(std::sin(slice * (i + 1)), 1.0f, std::cos(slice * (i + 1)));

        // side
        glNormal3f(std::sin(slice * (i + 0.5)), 0.0f, std::cos(slice * (i + 0.5)));
        glVertex3f(std::sin(slice * i), 0.0f, std::cos(slice * i));
        glVertex3f(std::sin(slice * (i + 1)), 0.0f, std::cos(slice * (i + 1)));
        glVertex3f(std::sin(slice * i), 1.0f, std::cos(slice * i));
        glVertex3f(std::sin(slice * (i + 1)), 1.0f, std::cos(slice * (i + 1)));
        glVertex3f(std::sin(slice * i), 1.0f, std::cos(slice * i));
        glVertex3f(std::sin(slice * (i + 1)), 0.0f, std::cos(slice * (i + 1)));
    }

    glEnd();
}

void light() {
  GLfloat light_specular[] = {0.6, 0.6, 0.6, 1.0};
  GLfloat light_diffuse[] = {0.6, 0.6, 0.6, 1.0};
  GLfloat light_ambient[] = { 0.4f, 0.4f, 0.4f, 1.0f };
  GLfloat light_position[] = {50.0, 75.0, 80.0, 1.0};

  // z buffer enable
  glEnable(GL_DEPTH_TEST);
  // enable lighting
  glEnable(GL_LIGHTING);
  glShadeModel(GL_SMOOTH);
  glEnable(GL_COLOR_MATERIAL);
  glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
  glEnable(GL_NORMALIZE);
  // set light property
  glEnable(GL_LIGHT0);
  glLightfv(GL_LIGHT0, GL_POSITION, light_position);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
  glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
}

int main() {
  initOpenGL();
  GLFWwindow* window = OpenGLContext::getWindow();

  // Init Camera helper
  Camera camera(glm::vec3(0, 2, 5));
  camera.initialize(OpenGLContext::getAspectRatio());
  // Store camera as glfw global variable for callbasks use
  glfwSetWindowUserPointer(window, &camera);

  // Main rendering loop
  while (!glfwWindowShouldClose(window)) {
    // Polling events.
    glfwPollEvents();
    // Update camera position and view
    camera.move(window);
    // GL_XXX_BIT can simply "OR" together to use.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    /// TO DO Enable DepthTest
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    // Projection Matrix
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(camera.getProjectionMatrix());
    // ModelView Matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(camera.getViewMatrix());

#ifndef DISABLE_LIGHT   
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glClearDepth(1.0f);
    light();
#endif

    /* TODO#4-2: Update joint degrees
     *       1. Finish keyCallback to detect key events
     *       2. Update jointx_degree if the correspond key is pressed
     * Note:
     *       You can use `ROTATE_SPEED` as the speed constant. 
     *       If the rotate speed is too slow or too fast, please change `ROTATE_SPEED` value
     */

    joint0_degree += ROTATE_SPEED * joint0_is_rotating;
    joint1_degree += ROTATE_SPEED * joint1_is_rotating;
    joint2_degree += ROTATE_SPEED * joint2_is_rotating;

    /* TODO#5: Catch the target object with robotic arm
     *       1. Calculate coordinate of the robotic arm endpoint
     *       2. Test if arm endpoint and the target object are close enough
     *       3. Update coordinate fo the target object to the arm endpoint
     *          if the space key is pressed
     * Hint: 
     *       GLM fransform API (https://glm.g-truc.net/0.9.4/api/a00206.html)
     * Note: 
     *       You might use `ANGEL_TO_RADIAN`
     *       and refer to `CATCH_POSITION_OFFSET` and `TOLERANCE`
     */

    glm::vec4 catch_detect_position(0.0f, 0.0f, 0.0f, 1.0f);
    glm::vec3 target_center = target_pos + glm::vec3(0.0f, TARGET_HEIGHT / 2, 0.0f);
    // (target_pos.x, target_pos.y + TARGET_HEIGHT / 2, target_pos.z);
    glm::mat4 trans(1.0f);

    // create transformation matrix
    trans = glm::rotate(trans, ANGEL_TO_RADIAN(joint0_degree), glm::vec3(0.0f, 1.0f, 0.0f));
    trans = glm::translate(trans, glm::vec3(0.0f, BASE_HEIGHT, 0.0f));
    trans = glm::translate(trans, glm::vec3(0.0f, ARM_LEN, 0.0f));
    trans = glm::translate(trans, glm::vec3(0.0f, JOINT_RADIUS, 0.0f));

    trans = glm::rotate(trans, ANGEL_TO_RADIAN(joint1_degree), glm::vec3(0.0f, 0.0f, 1.0f));
    trans = glm::translate(trans, glm::vec3(0.0f, JOINT_RADIUS, 0.0f));
    trans = glm::translate(trans, glm::vec3(0.0f, ARM_LEN, 0.0f));
    trans = glm::translate(trans, glm::vec3(0.0f, JOINT_RADIUS, 0.0f));

    trans = glm::rotate(trans, ANGEL_TO_RADIAN(joint2_degree), glm::vec3(0.0f, 0.0f, 1.0f));
    trans = glm::translate(trans, glm::vec3(0.0f, JOINT_RADIUS, 0.0f));
    trans = glm::translate(trans, glm::vec3(0.0f, ARM_LEN, 0.0f));
    trans = glm::translate(trans, glm::vec3(0.0f, CATCH_POSITION_OFFSET, 0.0f));
    
    // get catch detect position
    catch_detect_position = trans * catch_detect_position;
    
    // check if the position of arm endpoint can catch target
    can_catch = (
        (catch_detect_position.x - target_center.x) * (catch_detect_position.x - target_center.x) +
        (catch_detect_position.y - target_center.y) * (catch_detect_position.y - target_center.y) +
        (catch_detect_position.z - target_center.z) * (catch_detect_position.z - target_center.z)
        < TOLERANCE
    );

    // if can catch and is catch (pressing space), update the position of target
    if (can_catch && is_catching)
        target_pos = glm::vec3(
            catch_detect_position.x, 
            catch_detect_position.y - TARGET_HEIGHT / 2, 
            catch_detect_position.z
        );

    // physical falling simulation
    if (bonus && !(can_catch && is_catching)){
        target_pos.y += velocity;
        if (target_pos.y < 0){
            target_pos.y = 0;
            velocity = -velocity*0.5;
        }
        velocity += acceleration;
    }
    
    // Render a white board
    glPushMatrix();
    glScalef(3, 1, 3);
    glBegin(GL_TRIANGLE_STRIP);
    glColor3f(1.0f, 1.0f, 1.0f);
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-1.0f, 0.0f, -1.0f);
    glVertex3f(-1.0f, 0.0f, 1.0f);
    glVertex3f(1.0f, 0.0f, -1.0f);
    glVertex3f(1.0f, 0.0f, 1.0f);
    glEnd();
    glPopMatrix();

    /* TODO#2: Render a cylinder at target_pos
     *       1. Translate to target_pos
     *       2. Setup vertex color
     *       3. Setup cylinder scale
     *       4. Call drawUnitCylinder
     * Hint: 
     *       glTranslatef (https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/glTranslate.xml)
     *       glColor3f (https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/glColor.xml)
     *       glScalef (https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/glScale.xml)
     * Note:
     *       The coordinates of the cylinder are `target_pos`
     *       The cylinder's size can refer to `TARGET_RADIUS`, `TARGET_DIAMETER` and `TARGET_DIAMETER`
     *       The cylinder's color can refer to `RED`
     */
    
    glPushMatrix();
    glTranslatef(target_pos.x, target_pos.y, target_pos.z);
    if (bonus)
        glColor3f(WHITE);
    else
        glColor3f(RED);
    glScalef(TARGET_RADIUS, TARGET_HEIGHT, TARGET_RADIUS);
    drawUnitCylinder();
    glPopMatrix();

    /* TODO#3: Render the robotic arm
     *       1. Render the base
     *       2. Translate to top of the base
     *       3. Render an arm
     *       4. Translate to top of the arm
     *       5. Render the joint
     *       6. Translate and rotate to top of the join
     *       7. Repeat step 3-6
     * Hint:
     *       glPushMatrix/glPopMatrix (https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/glPushMatrix.xml)
     *       glRotatef (https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/glRotate.xml)
     * Note:
     *       The size of every component can refer to `Components size definition` section
     *       Rotate degree for joints are `joint0_degree`, `joint1_degree` and `joint2_degree`
     *       You may implement drawBase, drawArm and drawJoin first
     */

    // joint 0 (base)
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.0f);
    glRotatef(joint0_degree, 0.0f, 1.0f, 0.0f);
    glColor3f(GREEN);
    glScalef(BASE_RADIUS, BASE_HEIGHT, BASE_RADIUS);
    drawUnitCylinder();
    glScalef(1.0f / BASE_RADIUS, 1.0f / BASE_HEIGHT, 1.0f / BASE_RADIUS);

    // arm

    // move to arm 1 position
    glTranslatef(0.0f, BASE_HEIGHT, 0.0f);

    // arm 1
    glTranslatef(0.0f, 0.0f, 0.0f);
    glColor3f(BLUE);
    glScalef(ARM_RADIUS, ARM_LEN, ARM_RADIUS);
    drawUnitCylinder();
    glScalef(1.0f/ARM_RADIUS, 1.0f / ARM_LEN, 1.0f / ARM_RADIUS);

    // move to joint 1 position
    glTranslatef(0.0f, ARM_LEN + JOINT_RADIUS, 0.0f);
    
    // joint 1
    glTranslatef(0.0f, 0.0f, -ARM_RADIUS);
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
    glRotatef(joint1_degree, 0.0f, 1.0f, 0.0f);
    glColor3f(GREEN);
    glScalef(JOINT_RADIUS, JOINT_WIDTH, JOINT_RADIUS);
    drawUnitCylinder();
    glScalef(1.0f / JOINT_RADIUS, 1.0f / JOINT_WIDTH, 1.0f / JOINT_RADIUS);
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
    glTranslatef(0.0f, 0.0f, ARM_RADIUS);

    // move to arm 2 position
    glTranslatef(0.0f, JOINT_RADIUS, 0.0f);
    
    // arm 2
    glColor3f(BLUE);
    glScalef(ARM_RADIUS, ARM_LEN, ARM_RADIUS);
    glRotatef(0.0f, 1.0f, 0.0f, 0.0f);
    drawUnitCylinder();
    glScalef(1.0f / ARM_RADIUS, 1.0f / ARM_LEN, 1.0f / ARM_RADIUS);
    
    // move to joint 2 position
    glTranslatef(0.0f, ARM_LEN + JOINT_RADIUS, 0.0f);

    // joint 2
    glTranslatef(0.0f, 0.0f, -ARM_RADIUS);
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
    glRotatef(joint2_degree, 0.0f, 1.0f, 0.0f);
    glColor3f(GREEN);
    glScalef(JOINT_RADIUS, JOINT_WIDTH, JOINT_RADIUS);
    drawUnitCylinder();
    glScalef(1.0f / JOINT_RADIUS, 1.0f / JOINT_WIDTH, 1.0f / JOINT_RADIUS);
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
    glTranslatef(0.0f, 0.0f, ARM_RADIUS);

    // move to arm 3 position
    glTranslatef(0.0f, JOINT_RADIUS, 0.0f);

    // arm 3 
    glColor3f(BLUE);
    glScalef(ARM_RADIUS, ARM_LEN, ARM_RADIUS);
    drawUnitCylinder();
    glScalef(1.0f / ARM_RADIUS, 1.0f / ARM_LEN, 1.0f / ARM_RADIUS);

    glPopMatrix();

#ifdef __APPLE__
    // Some platform need explicit glFlush
    glFlush();
#endif
    glfwSwapBuffers(window);
  }
  return 0;
}
