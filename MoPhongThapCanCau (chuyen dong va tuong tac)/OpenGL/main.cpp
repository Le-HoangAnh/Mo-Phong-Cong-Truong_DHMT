/*Chương trình chiếu sáng Blinn-Phong (Phong sua doi) cho hình lập phương đơn vị, điều khiển quay bằng phím x, y, z, X, Y, Z.*/

#include "Angel.h"  /* Angel.h là file tự phát triển (tác giả Prof. Angel), có chứa cả khai báo includes glew và freeglut*/
#define PI 3.1415927

// remember to prototype
void generateGeometry(void);
void initGPUBuffers(void);
void shaderSetup(void);
void display(void);
void keyboard(unsigned char key, int x, int y);

typedef vec4 point4;
typedef vec4 color4;
using namespace std;


// Số các đỉnh của các tam giác
const int NumPoints = 36;

point4 points[NumPoints]; /* Danh sách các đỉnh của các tam giác cần vẽ*/
color4 colors[NumPoints]; /* Danh sách các màu tương ứng cho các đỉnh trên*/
vec3 normals[NumPoints]; /*Danh sách các vector pháp tuyến ứng với mỗi đỉnh*/

point4 vertices[8]; /* Danh sách 8 đỉnh của hình lập phương*/
color4 vertex_colors[8]; /*Danh sách các màu tương ứng cho 8 đỉnh hình lập phương*/

GLuint program;

GLfloat l = -1.0, r = 1.0, t = 1.0, b = -1.0, zNear = 1, zFar = 100, ttxebus = 0, aa=1.6, xx=0, yy=0;
GLfloat dr = 5.0 * M_PI / 180;
GLfloat theta[] = { 0,0,0 };
GLfloat alpha[] = { 0,0,0 };
GLfloat alpha3 = 0, alpha4 = 0, alpha5 = 0, alpha6 = 0, alpha1 = 0, alpha2 = 0, alpha7 = 0;
GLfloat zKeoTu = 0.0, ycuakinh = 0;

GLfloat craneRotationAngle = 0.0;  // Góc quay của trục cần cẩu


GLuint model_loc;
mat4 projection;
GLuint projection_loc;
mat4 view;
GLuint view_loc;
BOOLEAN quayquattran = false, quayquatcay = false, quaykimphut = true, quaykimgio = true;

GLfloat thetal = 0.0;
GLfloat xEye = 0.0, yEye = 0.0, zEye = 0.0; // tham sô tịnh tiến camera

mat4 model, modeltaycua;
mat4 quayBase;


void initCube()
{
	// Gán giá trị tọa độ vị trí cho các đỉnh của hình lập phương
	vertices[0] = point4(-0.5, -0.5, 0.5, 1.0);
	vertices[1] = point4(-0.5, 0.5, 0.5, 1.0);
	vertices[2] = point4(0.5, 0.5, 0.5, 1.0);
	vertices[3] = point4(0.5, -0.5, 0.5, 1.0);
	vertices[4] = point4(-0.5, -0.5, -0.5, 1.0);
	vertices[5] = point4(-0.5, 0.5, -0.5, 1.0);
	vertices[6] = point4(0.5, 0.5, -0.5, 1.0);
	vertices[7] = point4(0.5, -0.5, -0.5, 1.0);

	// Gán giá trị màu sắc cho các đỉnh của hình lập phương	
	vertex_colors[0] = color4(0.0, 0.0, 0.0, 1.0); // black
	vertex_colors[1] = color4(1.0, 0.0, 0.0, 1.0); // red
	vertex_colors[2] = color4(1.0, 1.0, 0.0, 1.0); // yellow
	vertex_colors[3] = color4(0.0, 1.0, 0.0, 1.0); // green
	vertex_colors[4] = color4(0.0, 0.0, 1.0, 1.0); // blue
	vertex_colors[5] = color4(1.0, 0.0, 1.0, 1.0); // magenta
	vertex_colors[6] = color4(1.0, 0.5, 0.0, 1.0); // orange
	vertex_colors[7] = color4(0.0, 1.0, 1.0, 1.0); // cyan
}
int Index = 0;
void quad(int a, int b, int c, int d)  /*Tạo một mặt hình lập phương = 2 tam giác, gán màu cho mỗi đỉnh tương ứng trong mảng colors*/
{
	vec4 u = vertices[b] - vertices[a];
	vec4 v = vertices[c] - vertices[b];
	vec3 normal = normalize(cross(u, v));

	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[b]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[c]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[c]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[d]; Index++;
}
void makeColorCube(void)  /* Sinh ra 12 tam giác: 36 đỉnh, 36 màu*/

{
	quad(1, 0, 3, 2);
	quad(2, 3, 7, 6);
	quad(3, 0, 4, 7);
	quad(6, 5, 1, 2);
	quad(4, 5, 6, 7);
	quad(5, 4, 0, 1);
}
void generateGeometry(void)
{
	initCube();
	makeColorCube();
}


void initGPUBuffers(void)
{
	// Tạo một VAO - vertex array object
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Tạo và khởi tạo một buffer object
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors) + sizeof(normals), NULL, GL_STATIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points), sizeof(colors), colors);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors), sizeof(normals), normals);


}

/* Khởi tạo các tham số chiếu sáng - tô bóng*/
point4 light_position(-2.0, 2.0, 0.0, 0.0); // vị trí điểm sáng 
color4 light_ambient(0.2, 0.2, 0.2, 1.0);
color4 light_diffuse(1.0, 1.0, 1.0, 1.0);
color4 light_specular(1.0, 1.0, 1.0, 1.0); //sáng chói

color4 material_ambient(1.0, 0.0, 1.0, 1.0);
color4 material_diffuse(1.0, 0.8, 0.0, 1.0);
color4 material_specular(1.0, 0.8, 0.0, 1.0);
float material_shininess = 100.0;

color4 ambient_product = light_ambient * material_ambient * vec4(1.1, 1.1, 1.1, 1.0);
color4 diffuse_product = light_diffuse * material_diffuse * vec4(1.2, 1.2, 1.2, 1.0);
color4 specular_product = light_specular * material_specular * vec4(1.5, 1.5, 1.5, 1.0);


void shaderSetup(void)
{
	// Nạp các shader và sử dụng chương trình shader
	program = InitShader("vshader1.glsl", "fshader1.glsl");   // hàm InitShader khai báo trong Angel.h
	glUseProgram(program);

	// Khởi tạo thuộc tính vị trí đỉnh từ vertex shader
	GLuint loc_vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(loc_vPosition);
	glVertexAttribPointer(loc_vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	GLuint loc_vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(loc_vColor);
	glVertexAttribPointer(loc_vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points)));

	GLuint loc_vNormal = glGetAttribLocation(program, "vNormal");
	glEnableVertexAttribArray(loc_vNormal);
	glVertexAttribPointer(loc_vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points) + sizeof(colors)));

	glUniform4fv(glGetUniformLocation(program, "AmbientProduct"), 1, ambient_product);
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);
	glUniform4fv(glGetUniformLocation(program, "SpecularProduct"), 1, specular_product);
	glUniform4fv(glGetUniformLocation(program, "LightPosition"), 1, light_position);
	glUniform1f(glGetUniformLocation(program, "Shininess"), material_shininess);

	model_loc = glGetUniformLocation(program, "Model");
	projection_loc = glGetUniformLocation(program, "Projection");
	view_loc = glGetUniformLocation(program, "View");

	glEnable(GL_DEPTH_TEST);
	glClearColor(1.0, 1.0, 1.0, 1.0);        /* Thiết lập màu trắng là màu xóa màn hình*/
}
vec4 mauvang = vec4(1.0, 1.0, 0.0, 1.0);
vec4 mautuong = vec4(0.0, 1, 1, 0.5);
vec4 maubanghe = vec4(1.0, 0.0, 0.0, 1.0);
vec4 maubanghe2 = vec4(1.0, 0.6, 0.6, 1.0);
vec4 maukhungcuaso = vec4(1, 0.59, 0.12, 1.0);
vec4 maucanhcuaso = vec4(1, 0, 0.5, 0.6);
vec4 maungankeo = vec4(0.75, 1, 0.08, 0.6);
vec4 mauden;
vec4 mautu = vec4(0.8, 0.5, 0.5, 1.0);
vec4 maukim = vec4(0, 0, 0, 1);


void matPhang(GLfloat x, GLfloat y, GLfloat z, mat4 mt, vec4 color_of_obj) {

	material_diffuse = color_of_obj;
	diffuse_product = light_diffuse * material_diffuse;
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);

	//camera
	point4 eye(0 + xEye, 1 + yEye, 4 + zEye, 1.0); // Camera position
	point4 at(-5 * sin(thetal) + xEye, 1 + yEye, -5 * cos(thetal) + zEye, 1.0); // Look-at point
	vec4 up(0, 1, 0, 1.0); // Up direction
	  // Hướng lên


	mat4 v = LookAt(eye, at, up); // camera
	glUniformMatrix4fv(view_loc, 1, GL_TRUE, v);

	mat4 ins = Scale(x, y, z);   // scale
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, quayBase * mt * ins);

	mat4 p = Frustum(l, r, b, t, zNear, zFar);  // view volumn
	glUniformMatrix4fv(projection_loc, 1, GL_TRUE, p);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}


void DatNen()
{
	model = Translate(0, -0.01, 0);
	matPhang(25, 0.02, 25, model, vec4(1, 1, 0.79, 0.3));
}

void Nha() { 
	quayBase = RotateY(theta[0]) * RotateX(theta[1])*Scale(0.8,1.1,0.8)*Translate(0,0,-3);
	DatNen();
}


//Mô phỏng cần cẩu
void thancancau() {

	for (int j = 0;j < 40;j++) {
		model = Translate(5, 2.75, 2) * RotateY(3+ j);
		matPhang(0.4, 5.5, 0.4, model, vec4(1.0, 1.0, 0, 0.6));
	}
}

void cabin() {
	// phan de
	model = Translate(5, 5.6, 2);
	matPhang(1.2, 0.2, 1.4, model, vec4(0.42, 0.45, 0.44, 0.5));

	// phan ghe lai
	model = Translate(4.9, 6.2, 2.4) ;
	matPhang(1.2, 1, 0.6, model, vec4(0.42, 0.44, 0.55, 0.5));

	// bo may 
	model = Translate(5, 6.2, 1.5);
	matPhang(1.1, 0.8, 0.8, model, vec4(0.85, 0.71, 0.69, 0.6));
	
	// phan ta dang sau
	model = Translate(6.9, 5.9, 1.8);
	matPhang(0.5, 1.2, 0.5, model, vec4(0.71, 0.71, 0.69, 0.6));
	
	// phan can cau
	model = Translate(3.15, 5.9, 1.8);
	matPhang(7.2, 0.2, 0.4, model, vec4(0.84, 0.75, 0, 0.3));
	//
	model = Translate(1.1+xx, 5.8, 1.8);
	matPhang(0.2, 0.05, 0.4, model, vec4(0, 0, 0, 0));
}

void phandaycau() {
 
	for (int i = 0; i < 100; i++)
	{
		model = Translate(1.2+xx, 5.0+yy , 2.0)* RotateY(2 + i);
		matPhang(0.06, aa, 0.06, model, vec4(0.26, 0.24, 0.27, 0.4));

		model = Translate(1.2+xx, 5.0+yy , 1.6) * RotateY(2 + i);
		matPhang(0.06, aa, 0.06, model, vec4(0.26, 0.24, 0.27, 0.4));

		model = Translate(1.0 + xx, 5.0+yy , 1.6) * RotateY(2 + i);
		matPhang(0.06, aa, 0.06, model, vec4(0.26, 0.24, 0.27, 0.4));

		model = Translate(1.0 + xx, 5.0+yy , 2.0) * RotateY(2 + i);
		matPhang(0.06, aa, 0.06, model, vec4(0.26, 0.24, 0.27, 0.4));

	}
}
void vatlieucau() {
	for (int i = 0; i < 100; i++)
	{
		model = Translate(1.1 + xx, 4.2 + 2* yy, 1.8) * RotateZ(3 + i);
		matPhang(0.2, 0.2, 0.4, model, vec4(0.91, 0.84, 0.07, 0.5));

	}
	model = Translate(1.2 + xx, 3.7 + 2 * yy, 1.6) * RotateZ(20);
	matPhang(0.06, 0.8, 0.06, model, maukim);

	model = Translate(1.2 + xx, 3.7 + 2 * yy, 2.0) * RotateZ(20);
	matPhang(0.06, 0.8, 0.06, model, maukim);

	model = Translate(0.9 + xx, 3.7 + 2 * yy, 1.6) * RotateZ(-20);
	matPhang(0.06, 0.8, 0.06, model, maukim);

	model = Translate(0.9 + xx, 3.7 + 2 * yy, 2.0) * RotateZ(-20);
	matPhang(0.06, 0.8, 0.06, model, maukim);


	model = Translate(1 + xx, 3.4 + 2 * yy, 1.8) * RotateY(30);
	matPhang(0.2, 0.2, 1.4, model, vec4(0.81, 0.57, 0, 0.4));

	model = Translate(1.2 + xx, 3.4 + 2 * yy, 1.8) * RotateY(30);
	matPhang(0.2, 0.2, 1.4, model, vec4(0.81, 0.57, 0, 0.4));

	model = Translate(1.1 + xx, 3.6 + 2 * yy, 1.8) * RotateY(30);
	matPhang(0.2, 0.2, 1.4, model, vec4(0.81, 0.57, 0, 0.4));
}

void cancau() {
	quayBase = RotateY(theta[0]) * RotateX(theta[1]) * Translate(2, 0, -3);
	thancancau();

	quayBase = quayBase * Translate(5, 5.6, 2) * RotateY(craneRotationAngle) * Translate(-5, -5.6, -2);
	cabin();
	phandaycau();
	vatlieucau();
}



//--------------------------------------------------------
//tổng  hợp các thiết bị trong phòng 
void congtruong() {

	Nha();
	cancau();
}
//--------------------------------------------------------

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	congtruong();
	
	glutSwapBuffers();
}


void keyboard(unsigned char key, int x, int y)
{
	// keyboard handler

	switch (key) {
	case 033:			// 033 is Escape key octal value
		exit(1);		// quit program
		break;

		//điều khiển camera
	case 'd': // xoay sang phải
		thetal -= 0.1;
		glutPostRedisplay();
		break;
	case 'a': // xoay sang trái
		thetal += 0.1;
		glutPostRedisplay();
		break;

	case 'p': // Di chuyển camera xuống dưới
		yEye -= 0.1;
		break;
	case 'P': // Di chuyển camera lên trên
		yEye += 0.1;
		break;

	case 'i': // Di chuyển camera sang phải
		xEye -= 0.1;
		break;
	case 'I': // Di chuyển camera sang trái
		xEye += 0.1;
		break;

	case 'w': //Di chuyển camera tiến lên
		zEye -= 0.1;
		break;
	case 's': //Di chuyển camera lùi xuống
		zEye += 0.1;
		break;

	case 'q':
		theta[1] += 5;
		glutPostRedisplay();
		break;
	case 'Q':
		theta[1] -= 5;
		glutPostRedisplay();
		break;

	//tăng giảm view volumn
	case 'W':
		zNear *= 1.05; zFar *= 1.05;
		break;
	case 'S':
		zNear *= 0.95; zFar *= 0.95; 
		break;
	
	case 't': thetal += dr; break;
	case 'T': thetal -= dr; break;



	//Cần cẩu
	case 'z': //di chuyen day can cau di xuong
		aa += 0.1;
		yy -= 0.05;
		break;
	case 'Z': //di chuyen day can cau di len
		aa -= 0.1;
		yy += 0.05;
		break;

	case 'x': //di chuyen truc can cau vao trong
		xx += 0.1;
		break;
	case 'X': //di chuyen truc can cau ra ngoai
		xx -= 0.1;
		break;

	}
	
	glutPostRedisplay();	
}

//Chuyển động của cần cẩu
void updateCraneRotation() {
	craneRotationAngle += 0.5;  // Tốc độ xoay

	if (craneRotationAngle > 360.0) {
		craneRotationAngle -= 360.0;  // Đặt lại góc khi vượt quá 360 độ
	}
	glutPostRedisplay();  // Yêu cầu vẽ lại sau khi cập nhật góc quay
}

void updateScene() {
	updateCraneRotation();   // Cập nhật góc quay cần cẩu
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(640, 640);
	glutInitWindowPosition(100, 150);
	glutCreateWindow("Working Room");

	glewInit();

	generateGeometry();
	initGPUBuffers();
	shaderSetup();

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(updateScene);  // Cập nhật cần cẩu
	glutMainLoop();
	return 0;
}




 