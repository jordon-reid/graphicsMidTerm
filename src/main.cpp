#include "Window.h"
#include "Shader.h"
#include <cstdlib>  // For rand
#include <cstdio>   // For printf
#include <ctime>    // For time

RMAPI float Random(float min, float max)
{
    return min + (rand() / ((float)RAND_MAX / (max - min)));
}

const int NUM_VERTICES = 30000;

static const Vector2 TRIANGLE_POSITIONS[3]
{
    {-1.0f, -1.0f},
    {1.0f, -1.0f},
    {0.0f, 1.0f}
};
static const Vector3 TRIANGLE_COLOURS[3]
{
    {1.0f, 0.0f, 0.0f},
    {0.0f, 1.0f, 0.0f},
    {0.0f, 0.0f, 1.0f}
};
//static const int line_vertex_count = 8;
//static const Vector2 line_vertex_positions[line_vertex_count]
//{
//    { -1.0f,  -1.0f },   // bottom-left
//    {  1.0f,  -1.0f },   // bottom-right
//
//    {  1.0f, -1.0f },   // bottom-right
//    {  1.0f,  1.0f },   // top-right
//
//    {   1.0f,  1.0f },   // top-right
//    {  -1.0f,  1.0f },   // top-left
//
//    { -1.0f,   1.0f },   // top-left
//    { -1.0f,  -1.0f }    // bottom-left
//};
//
//static const Vector3 line_vertex_colors[line_vertex_count]
//{
//    Vector3UnitX,
//    Vector3UnitY,
//
//    Vector3UnitZ,
//    Vector3Ones,
//
//    Vector3UnitX,
//    Vector3UnitY,
//
//    Vector3UnitZ,
//    Vector3Ones
//};

int main()
{
    // Random number generator example -- % 3 generates random numbers between 0, 1, and 2.
    // srand "seeds" the random number generator, so your "random sequence" isn't the same every program run.
    srand(time(nullptr));
    
    Vector2 sierpinski_positions[NUM_VERTICES];
    Vector3 sierpinski_colours[NUM_VERTICES];
    Vector2 current_position = TRIANGLE_POSITIONS[0];

    for (int i = 0; i < NUM_VERTICES; ++i)
    {
        int n = rand() % 3;
        current_position = Vector2Lerp(current_position, TRIANGLE_POSITIONS[n], 0.5f);
        sierpinski_positions[i] = current_position;
        sierpinski_colours[i] = TRIANGLE_COLOURS[n];
    }

    CreateWindow(800, 800, "Graphics 1");
    
    GLuint a2_lines_vert = CreateShader(GL_VERTEX_SHADER, "./assets/shaders/a2_lines.vert");
    GLuint a2_lines_frag = CreateShader(GL_FRAGMENT_SHADER, "./assets/shaders/a2_lines.frag");
    GLuint a2_points_shader = CreateProgram(a2_lines_vert, a2_lines_frag);

    GLuint vbo_point_positions;
    glGenBuffers(1, &vbo_point_positions);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_point_positions);
    glBufferData(GL_ARRAY_BUFFER, sizeof(sierpinski_positions), sierpinski_positions, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

    GLuint vbo_point_colours;
    glGenBuffers(1, &vbo_point_colours);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_point_colours);
    glBufferData(GL_ARRAY_BUFFER, sizeof(sierpinski_colours), sierpinski_colours, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

    GLuint vao_points;
    glGenVertexArrays(1, &vao_points);
    glBindVertexArray(vao_points);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_point_positions);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vector2), nullptr);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_point_colours);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3), nullptr);
    
    glBindVertexArray(GL_NONE);

    while (!WindowShouldClose())
    {
        if (IsKeyPressed(KEY_ESCAPE))
            SetWindowShouldClose(true);

        Matrix proj = MatrixOrtho(-1.0f, 1.0f, -1.0f, 1.0f, 0.01f, 100.0f);
        Matrix view = MatrixLookAt({ 0.0f, 0.0f, 10.0f }, { 0.0f, 0.0f, 0.0f }, Vector3UnitY);
        Matrix world = MatrixIdentity();
        Matrix mvp = world * view * proj;

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glPointSize(1.0f);
        glBindVertexArray(vao_points);

        int loc_mvp = glGetUniformLocation(a2_points_shader, "u_mvp");
        glUseProgram(a2_points_shader);
        glUniformMatrix4fv(loc_mvp, 1, GL_FALSE, MatrixToFloat(mvp));
        glDrawArrays(GL_POINTS, 0, NUM_VERTICES);
        glUseProgram(GL_NONE);
        
        glBindVertexArray(GL_NONE);
    
        BeginGui();
        //ImGui::ShowDemoWindow(nullptr);
        EndGui();

        Loop();
    }

    glDeleteVertexArrays(1, &vao_points);
    glDeleteBuffers(1, &vbo_point_positions);
    glDeleteProgram(a2_points_shader);
    glDeleteShader(a2_lines_frag);
    glDeleteShader(a2_lines_vert);

    DestroyWindow();
    return 0;
}
