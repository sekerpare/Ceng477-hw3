#include "EclipseMap.h"

using namespace std;

struct vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texture;

    vertex() {}

    vertex(const glm::vec3 &position, const glm::vec3 &normal, const glm::vec2 &texture) : position(position),
                                                                                           normal(normal),
                                                                                           texture(texture) {}
};

struct triangle {
    int vertex1;
    int vertex2;
    int vertex3;

    triangle() {}

    triangle(const int &vertex1, const int &vertex2, const int &vertex3) : vertex1(vertex1), vertex2(vertex2),
                                                                           vertex3(vertex3) {}
};

pair<vector<float>, vector<unsigned int> > EclipseMap::generateSphereVerticesAndIndices(float radius, int horizontalSplitCount, int verticalSplitCount, float startx , float starty ,float startz )
{
    vector<float> vertices;
    vector<unsigned int> indices;
    unsigned int k1, k2;
    
    for(int vertical_step=0 ; vertical_step<=verticalSplitCount ; vertical_step++ ){
        float beta = PI * (float) vertical_step / (float) verticalSplitCount; //odev metninde verilmis
        for(int horizontal_step=0 ; horizontal_step<=horizontalSplitCount ; horizontal_step++){
            float alpha= 2 * PI * (float) horizontal_step / (float) horizontalSplitCount;
            float z = radius * cosf(beta);
            float y = radius * sinf(beta) * sinf(alpha);
            float x = radius * sinf(beta) * cosf(alpha);
            //once verticeleri pushluyorum
            vertices.push_back(x+startx);
            vertices.push_back(y+starty);
            vertices.push_back(z+startz);
            //sonra normalleri pushluyorum
            vertices.push_back(x/radius);
            vertices.push_back(y/radius);
            vertices.push_back(z/radius);
            //sonra texture coordinateleri pushluyorum
            vertices.push_back((float)horizontal_step/horizontalSplitCount);
            vertices.push_back((float)vertical_step  /verticalSplitCount);

        }
    }
    
    for (int i = 0; i < verticalSplitCount; i++)
    {
        k1 = i * (horizontalSplitCount + 1);
        k2 = k1 + horizontalSplitCount + 1;
        for (int j = 0; j < horizontalSplitCount; j++)
        {
            if (i != 0)
            {
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);
            }
            if (i != horizontalSplitCount - 1)
            {
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
            k1++;
            k2++;
        }
    }
    return make_pair(vertices, indices);
}

void EclipseMap::Update_camera(GLuint ID){
    //ProjectionMatrix;
    //once matrixi bul
    cameraPosition += (cameraDirection * speed) ;
    glm::mat4 newProjectionMatrix = glm::perspective(glm::radians(projectionAngle),aspectRatio,near,far);
    //sonra shaderi updatele
    GLint PM = glGetUniformLocation(ID, "ProjectionMatrix");
    glUniformMatrix4fv(PM,1,GL_FALSE, &newProjectionMatrix[0][0]);
    //ViewMatrix;
    glm::mat4 newViewMatrix = glm::lookAt(cameraPosition,cameraDirection+cameraPosition,cameraUp);
    GLint VM = glGetUniformLocation(ID, "ViewMatrix");
    glUniformMatrix4fv(VM,1,GL_FALSE, &newViewMatrix[0][0]);
    //MVP; //
    glm::mat4 newMVP   = newProjectionMatrix * newViewMatrix ;//
    GLint MVPID = glGetUniformLocation(ID, "MVP");
    glUniformMatrix4fv(MVPID,1,GL_FALSE, &newMVP[0][0]);
    //NormalMatrix;
    glm::mat4 newNormalMatrix= glm::transpose(glm::inverse(newMVP));
    GLint NM = glGetUniformLocation(ID, "NormalMatrix");
    glUniformMatrix4fv(NM,1,GL_FALSE, &newNormalMatrix[0][0]);
}

void EclipseMap::Update_uniform_variables(GLuint ID){
    //lightPosition//
    GLint LP = glGetUniformLocation(ID, "lightPosition");
    glUniform3fv(LP,1, &lightPos[0]);
    //cameraPosition//
    GLint CP = glGetUniformLocation(ID, "cameraPosition");
    glUniform3fv(CP,1, &cameraPosition[0]);
    //textureOffset; 
    GLint TO = glGetUniformLocation(ID, "textureOffset");
    glUniform1f(TO, textureOffset);
    //heightFactor;//
    GLint HF = glGetUniformLocation(ID, "heightFactor");
    glUniform1f(HF, heightFactor);
    //imageWidth;
    GLint IW = glGetUniformLocation(ID, "imageWidth");
    glUniform1f(IW, imageWidth);
    //imageHeight;
    GLint IH = glGetUniformLocation(ID, "imageHeight");
}

void EclipseMap::Configure_Buffers_World(){
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    // This part is important !!! Bahadir burayı sen de kontrol et lutfen !
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    glBufferData(GL_ARRAY_BUFFER, worldVertices.size()*sizeof(float) , worldVertices.data() , GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, worldIndices.size()*sizeof(unsigned int) , worldIndices.data() , GL_STATIC_DRAW);

    glEnableVertexAttribArray(0); //vertex  icin 
    glEnableVertexAttribArray(1); //normal  icin 
    glEnableVertexAttribArray(2); //normal  icin 

    //vertex  icin.
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,sizeof(vertex), 0);
    //normal  icin
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,sizeof(vertex), (void*)(sizeof(GL_FLOAT)*3 ));
    //normal  icin
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE,sizeof(vertex), (void*)(sizeof(GL_FLOAT)*6 ));
}

void EclipseMap::Configure_Buffers_Moon(){
    glGenVertexArrays(1, &moonVAO);
    glGenBuffers(1, &moonVBO);
    glGenBuffers(1, &moonEBO);

    glBindVertexArray(moonVAO);
    glBindBuffer(GL_ARRAY_BUFFER, moonVBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, moonEBO);

    glBufferData(GL_ARRAY_BUFFER, moonVertices.size()*sizeof(float) , moonVertices.data() , GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, moonIndices.size()*sizeof(unsigned int) , moonIndices.data() , GL_STATIC_DRAW);

    glEnableVertexAttribArray(0); //vertex  icin 
    glEnableVertexAttribArray(1); //normal  icin 
    glEnableVertexAttribArray(2); //normal  icin 

    //vertex  icin.
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,sizeof(vertex), 0);
    //normal  icin
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,sizeof(vertex), (void*)(sizeof(GL_FLOAT)*3 ));
    //normal  icin
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE,sizeof(vertex), (void*)(sizeof(GL_FLOAT)*6 ));
}


void EclipseMap::Render(const char *coloredTexturePath, const char *greyTexturePath, const char *moonTexturePath) {
    // Open window
    GLFWwindow *window = openWindow(windowName, screenWidth, screenHeight);

    // Moon commands
    // Load shaders
    GLuint moonShaderID = initShaders("moonShader.vert", "moonShader.frag");

    initMoonColoredTexture(moonTexturePath, moonShaderID);
    
    // TODO: Set moonVertices
    pair<vector<float>, vector<unsigned int> > moonVerticesAndIndices = generateSphereVerticesAndIndices(moonRadius,horizontalSplitCount,verticalSplitCount,0,2600,0);     
    moonVertices = moonVerticesAndIndices.first;
    moonIndices  = moonVerticesAndIndices.second;
    
    // TODO: Configure Buffers
    Configure_Buffers_Moon();
    
    // World commands
    // Load shaders
    GLuint worldShaderID = initShaders("worldShader.vert", "worldShader.frag");

    initColoredTexture(coloredTexturePath, worldShaderID);
    initGreyTexture(greyTexturePath, worldShaderID);

    // TODO: Set worldVertices
    pair<vector<float>, vector<unsigned int> > worldVerticesAndIndices = generateSphereVerticesAndIndices(radius, horizontalSplitCount,verticalSplitCount,0,0,0);

    worldVertices = worldVerticesAndIndices.first;
    worldIndices = worldVerticesAndIndices.second;
    
    // TODO: Configure Buffers
    Configure_Buffers_World();
    
    // Enable depth test
    glEnable(GL_DEPTH_TEST);

    // Main rendering loop
    do {
        glViewport(0, 0, screenWidth, screenHeight);

        glClearStencil(0);
        glClearDepth(1.0f);
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        // TODO: Handle key presses
        handleKeyPress(window);

        // TODO: Use moonShaderID program
        glUseProgram(moonShaderID);

        //* TODO: Manipulate rotation variables
        ModelMoon  = rot_moon * ModelMoon;
        GLint MMM  = glGetUniformLocation(moonShaderID, "ModelMatrix");
        glUniformMatrix4fv(MMM,1,GL_FALSE, &ModelMoon[0][0]);

        SelfRot    = rotation * SelfRot;
        GLint SRM  = glGetUniformLocation(moonShaderID, "SelfRotationMatrix");
        glUniformMatrix4fv(SRM,1,GL_FALSE, &SelfRot[0][0]);
        
        // TODO: Bind textures
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureColor);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, textureGrey);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, moonTextureColor);

    
        //* TODO: Update camera at every frame
        Update_camera(moonShaderID);
        
        //* TODO: Update uniform variables at every frame
        Update_uniform_variables(moonShaderID);
        
        // TODO: Bind moon vertex array
        glBindVertexArray(moonVAO);        

        // TODO: Draw moon object
        glDrawElements(GL_TRIANGLES, moonIndices.size(), GL_UNSIGNED_INT, 0);
        
        /*************************/

        // TODO: Use worldShaderID program
        glUseProgram(worldShaderID);
        
        ModelWorld = rotation * ModelWorld;
        GLint WMM  = glGetUniformLocation(worldShaderID, "ModelMatrix");
        glUniformMatrix4fv(WMM,1,GL_FALSE, &ModelWorld[0][0]);

        //* TODO: Update camera at every frame
        Update_camera(worldShaderID);

        //* TODO: Update uniform variables at every frame
        Update_uniform_variables(worldShaderID);
        
        // TODO: Bind world vertex array
        glBindVertexArray(VAO);

        // TODO: Draw world object
        glDrawElements(GL_TRIANGLES, worldIndices.size(), GL_UNSIGNED_INT, 0);

        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    } while (!glfwWindowShouldClose(window));

    // Delete buffers
    glDeleteBuffers(1, &moonVAO);
    glDeleteBuffers(1, &moonVBO);
    glDeleteBuffers(1, &moonEBO);
    
    // Delete buffers
    glDeleteBuffers(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
   
    glDeleteProgram(moonShaderID);
    glDeleteProgram(worldShaderID);

    // Close window
    glfwTerminate();
}

void EclipseMap::handleKeyPress(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS ||glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_REPEAT) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
    else if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS ) {
        //DONE
        heightFactor += 10;
    }
    else if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS ) {
        //DONE
        heightFactor -= 10;
    }
    /*You will implement a camera flight mode to be able to fly over the visualized terrain. The camera will have a gaze direction, which will be modeled by two angles for pitch and yaw.
    Pitch and yaw angles are very closely related to the phi and theta angles that we have used to represent a sphere with parametric equations.
    The user will be able to change pitch with W and S keys where changing is 0.05 unit.
    W and S keys rotates gaze around the left vector. 
    Do not forget to update up and left vector with normalizing later. 
    The camera will also move with some speed at every frame.*/
    else if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS ) {
        //WRONG
        pitch += 0.05;
        glm::vec3 left_vector = glm::normalize(glm::cross(cameraUp,cameraDirection));
        glm::mat4 rot         = glm::rotate(I, 0.05f, left_vector);
        cameraDirection       = rot * glm::vec4(cameraDirection,1.0f) ;
        cameraUp = glm::normalize(glm::cross(cameraDirection,left_vector)) ;
    }
    else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS ) {
        //WRONG
        pitch -= 0.05;
        glm::vec3 left_vector = glm::normalize(glm::cross(cameraUp,cameraDirection));
        glm::mat4 rot         = glm::rotate(I, -0.05f, left_vector);
        cameraDirection       = rot * glm::vec4(cameraDirection,1.0f) ;
        cameraUp = glm::normalize(glm::cross(cameraDirection,left_vector)) ;
    }
    //and change yaw with A and D keys 
    //A and D keys rotates the gaze around the up vector.
    else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS ) {
        //WRONG
        yaw += 0.05f;
        glm::mat4 rot         = glm::rotate(I, 0.05f,cameraUp);
        cameraDirection       = rot * glm::vec4(cameraDirection,1.0f) ;
    }
    else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS ) {
        //WRONG
        yaw -= 0.05f;
        glm::mat4 rot         = glm::rotate(I, -0.05f,cameraUp);
        cameraDirection       = rot * glm::vec4(cameraDirection,1.0f) ;
    }
    else if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS ) {
        //DONE
        //The speed will increase or decrease with 0.01 by pressing the Y and H keys on the keyboard.
        speed += 0.01;
    }
    else if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS) {
        //DONE
        speed -= 0.01;
    }
    else if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
        //DONE
        speed=0;
    }
    else if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS ) {
        //DONE
        projectionAngle = 45;
        aspectRatio = 1;
        near = 0.1;
        far = 10000;
        pitch = startPitch;
        yaw = startYaw;
        speed = startSpeed;
        heightFactor = 80;
        textureOffset = 0;
        orbitDegree = 0;
        cameraUp = cameraStartUp;
        cameraPosition = cameraStartPosition;
        cameraDirection = cameraStartDirection;
    }
    else if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS ) {
        // TODO
        //Window size is initially (1000, 1000) and it should be switched to full-screen mode with the key P. 
        //Besides, your program should support resizing window operation with the frame.
        if(displayFormat == displayFormatOptions::fullScreen) {
            GLFWmonitor *monitor = glfwGetPrimaryMonitor();
            const GLFWvidmode *mode = glfwGetVideoMode(monitor);
            screenWidth = defaultScreenWidth;
            screenHeight = defaultScreenHeight;
            glfwSetWindowMonitor(window, NULL, 1, 31, defaultScreenWidth, defaultScreenHeight, mode->refreshRate);
            displayFormat = displayFormatOptions::windowed;
        }
        else {
            GLFWmonitor *monitor = glfwGetPrimaryMonitor();
            const GLFWvidmode *mode = glfwGetVideoMode(monitor);
            
            screenWidth = mode->width;
            screenHeight= mode->height;
            glfwSetWindowMonitor(window, monitor, 1, 31, mode->width, mode->height, mode->refreshRate);
            displayFormat = displayFormatOptions::fullScreen;
        }
        aspectRatio = (float) screenWidth/(float) screenHeight;
    }

}

GLFWwindow *EclipseMap::openWindow(const char *windowName, int width, int height) {
    if (!glfwInit()) {
        getchar();
        return 0;
    }

    const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow *window = glfwCreateWindow(width, height, windowName, NULL, NULL);
    glfwSetWindowMonitor(window, NULL, 1, 31, screenWidth, screenHeight, mode->refreshRate);

    if (window == NULL) {
        getchar();
        glfwTerminate();
        return 0;
    }

    glfwMakeContextCurrent(window);

    glewExperimental = true;
    if (glewInit() != GLEW_OK) {
        getchar();
        glfwTerminate();
        return 0;
    }

    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    glClearColor(0, 0, 0, 0);

    return window;
}


void EclipseMap::initColoredTexture(const char *filename, GLuint shader) {
    int width, height;
    glGenTextures(1, &textureColor);
    cout << shader << endl;
    glBindTexture(GL_TEXTURE_2D, textureColor);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                    GL_CLAMP_TO_EDGE);    // set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    unsigned char *raw_image = NULL;
    int bytes_per_pixel = 3;   /* or 1 for GRACYSCALE images */
    int color_space = JCS_RGB; /* or JCS_GRAYSCALE for grayscale images */

    /* these are standard libjpeg structures for reading(decompression) */
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;

    /* libjpeg data structure for storing one row, that is, scanline of an image */
    JSAMPROW row_pointer[1];

    FILE *infile = fopen(filename, "rb");
    unsigned long location = 0;
    int i = 0, j = 0;

    if (!infile) {
        printf("Error opening jpeg file %s\n!", filename);
        return;
    }
    printf("Texture filename = %s\n", filename);

    /* here we set up the standard libjpeg error handler */
    cinfo.err = jpeg_std_error(&jerr);
    /* setup decompression process and source, then read JPEG header */
    jpeg_create_decompress(&cinfo);
    /* this makes the library read from infile */
    jpeg_stdio_src(&cinfo, infile);
    /* reading the image header which contains image information */
    jpeg_read_header(&cinfo, TRUE);
    /* Start decompression jpeg here */
    jpeg_start_decompress(&cinfo);

    /* allocate memory to hold the uncompressed image */
    raw_image = (unsigned char *) malloc(cinfo.output_width * cinfo.output_height * cinfo.num_components);
    /* now actually read the jpeg into the raw buffer */
    row_pointer[0] = (unsigned char *) malloc(cinfo.output_width * cinfo.num_components);
    /* read one scan line at a time */
    while (cinfo.output_scanline < cinfo.image_height) {
        jpeg_read_scanlines(&cinfo, row_pointer, 1);
        for (i = 0; i < cinfo.image_width * cinfo.num_components; i++)
            raw_image[location++] = row_pointer[0][i];
    }

    height = cinfo.image_height;
    width = cinfo.image_width;


    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, raw_image);
   

    imageWidth = width;
    imageHeight = height;

    glGenerateMipmap(GL_TEXTURE_2D);

    glUseProgram(shader); // don't forget to activate/use the shader before setting uniforms!
    // either set it manually like so:

    glUniform1i(glGetUniformLocation(shader, "TexColor"), 0);
    /* wrap up decompression, destroy objects, free pointers and close open files */
    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    free(row_pointer[0]);
    free(raw_image);
    fclose(infile);

}

void EclipseMap::initGreyTexture(const char *filename, GLuint shader) {

    glGenTextures(1, &textureGrey);
    glBindTexture(GL_TEXTURE_2D, textureGrey);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                    GL_CLAMP_TO_EDGE);    // set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    int width, height;

    unsigned char *raw_image = NULL;
    int bytes_per_pixel = 3;   /* or 1 for GRACYSCALE images */
    int color_space = JCS_RGB; /* or JCS_GRAYSCALE for grayscale images */

    /* these are standard libjpeg structures for reading(decompression) */
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;

    /* libjpeg data structure for storing one row, that is, scanline of an image */
    JSAMPROW row_pointer[1];

    FILE *infile = fopen(filename, "rb");
    unsigned long location = 0;
    int i = 0, j = 0;

    if (!infile) {
        printf("Error opening jpeg file %s\n!", filename);
        return;
    }
    printf("Texture filename = %s\n", filename);

    /* here we set up the standard libjpeg error handler */
    cinfo.err = jpeg_std_error(&jerr);
    /* setup decompression process and source, then read JPEG header */
    jpeg_create_decompress(&cinfo);
    /* this makes the library read from infile */
    jpeg_stdio_src(&cinfo, infile);
    /* reading the image header which contains image information */
    jpeg_read_header(&cinfo, TRUE);
    /* Start decompression jpeg here */
    jpeg_start_decompress(&cinfo);

    /* allocate memory to hold the uncompressed image */
    raw_image = (unsigned char *) malloc(cinfo.output_width * cinfo.output_height * cinfo.num_components);
    /* now actually read the jpeg into the raw buffer */
    row_pointer[0] = (unsigned char *) malloc(cinfo.output_width * cinfo.num_components);
    /* read one scan line at a time */
    while (cinfo.output_scanline < cinfo.image_height) {
        jpeg_read_scanlines(&cinfo, row_pointer, 1);
        for (i = 0; i < cinfo.image_width * cinfo.num_components; i++)
            raw_image[location++] = row_pointer[0][i];
    }

    height = cinfo.image_height;
    width = cinfo.image_width;

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, raw_image);
  



    glGenerateMipmap(GL_TEXTURE_2D);

    glUseProgram(shader); // don't forget to activate/use the shader before setting uniforms!
    // either set it manually like so:

    glUniform1i(glGetUniformLocation(shader, "TexGrey"), 1);
    /* wrap up decompression, destroy objects, free pointers and close open files */
    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    free(row_pointer[0]);
    free(raw_image);
    fclose(infile);

}

void EclipseMap::initMoonColoredTexture(const char *filename, GLuint shader) {
    int width, height;
    glGenTextures(1, &moonTextureColor);
    cout << shader << endl;
    glBindTexture(GL_TEXTURE_2D, moonTextureColor);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                    GL_CLAMP_TO_EDGE);    // set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    unsigned char *raw_image = NULL;
    int bytes_per_pixel = 3;   /* or 1 for GRACYSCALE images */
    int color_space = JCS_RGB; /* or JCS_GRAYSCALE for grayscale images */

    /* these are standard libjpeg structures for reading(decompression) */
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;

    /* libjpeg data structure for storing one row, that is, scanline of an image */
    JSAMPROW row_pointer[1];

    FILE *infile = fopen(filename, "rb");
    unsigned long location = 0;
    int i = 0, j = 0;

    if (!infile) {
        printf("Error opening jpeg file %s\n!", filename);
        return;
    }
    printf("Texture filename = %s\n", filename);

    /* here we set up the standard libjpeg error handler */
    cinfo.err = jpeg_std_error(&jerr);
    /* setup decompression process and source, then read JPEG header */
    jpeg_create_decompress(&cinfo);
    /* this makes the library read from infile */
    jpeg_stdio_src(&cinfo, infile);
    /* reading the image header which contains image information */
    jpeg_read_header(&cinfo, TRUE);
    /* Start decompression jpeg here */
    jpeg_start_decompress(&cinfo);

    /* allocate memory to hold the uncompressed image */
    raw_image = (unsigned char *) malloc(cinfo.output_width * cinfo.output_height * cinfo.num_components);
    /* now actually read the jpeg into the raw buffer */
    row_pointer[0] = (unsigned char *) malloc(cinfo.output_width * cinfo.num_components);
    /* read one scan line at a time */
    while (cinfo.output_scanline < cinfo.image_height) {
        jpeg_read_scanlines(&cinfo, row_pointer, 1);
        for (i = 0; i < cinfo.image_width * cinfo.num_components; i++)
            raw_image[location++] = row_pointer[0][i];
    }

    height = cinfo.image_height;
    width = cinfo.image_width;


    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, raw_image);
   

    imageWidth = width;
    imageHeight = height;

    glGenerateMipmap(GL_TEXTURE_2D);

    glUseProgram(shader); // don't forget to activate/use the shader before setting uniforms!
    // either set it manually like so:

    glUniform1i(glGetUniformLocation(shader, "MoonTexColor"), 2);
    /* wrap up decompression, destroy objects, free pointers and close open files */
    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    free(row_pointer[0]);
    free(raw_image);
    fclose(infile);

}
