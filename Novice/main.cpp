#include <Novice.h>
#include <cassert>
#include <cmath>
#include <imgui.h>

const char kWindowTitle[] = "GC2A_07_ソウ_チョウキ_MT3";

#pragma region 自分の型といろんな定数
// 自分の型
struct Vector2 { // ベクトル2
	float x;
	float y;
};
struct Vector3 { // ベクトル3
	float x;
	float y;
	float z;
};
struct Matrix4x4 { // 4x4行列
	float m[4][4];
};
struct Segment { // 線分(Vector3 始点、Vector3 終点への差分ベクトル)
	Vector3 origin;
	Vector3 diff;
};
struct Sphere { // 球(Vector3 中心、float 半径)
	Vector3 center;
	float radius;
};
struct Plane { // 平面(Vector3 法線、float 距離)
	Vector3 normal;
	float distance;
};

const float kWindowWidth = 1280; // スクリーンの横
const float kWindwoHeight = 720; // スクリーンの縦
#pragma endregion

#pragma region 数学計算
// 加算
Vector3 Add(const Vector3& v1, const Vector3& v2);
// 減算
Vector3 Subtract(const Vector3& v1, const Vector3& v2);
// スカラー倍
Vector3 Multiply(float scalar, const Vector3& v);
// 内積
float Dot(const Vector3& v1, const Vector3& v2);
// クロス積(外積)
Vector3 Cross(const Vector3& v1, const Vector3& v2);
// 長さ
float Length(const Vector3& v);
// 正規化
Vector3 Normalize(const Vector3& v);

//  アフィン変換
Matrix4x4 MakeAffineMatrix(Vector3 scale, Vector3 rotation, Vector3 translation);
// 掛け算
Matrix4x4 Multiply(Matrix4x4 m1, Matrix4x4 m2);
// 逆行列
Matrix4x4 Inverse(const Matrix4x4& m);
// 転置行列
Matrix4x4 Transpose(const Matrix4x4& m);
// 正射影行列
Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip);
// 透視投影行列
Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip);
// ビューポート変換行列
Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth);

// 座標変換
Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix);
// 正射影ベクトル
Vector3 Project(const Vector3& v1, const Vector3& v2);
// 最近接点
Vector3 ClosestPoint(const Vector3& point, const Segment& segment);
// 垂直ベクトル
Vector3 Perpendicular(const Vector3& vector);
#pragma endregion

#pragma region 物理計算
bool IsCollision(const Sphere& s1, const Sphere& s2);
bool IsCollision(const Sphere& sphere, const Plane& plane);
bool IsCollision(const Segment& segment, const Plane& plane);
#pragma endregion

#pragma region 工具
// 行列、ベクトルの値をスクリーンに出す
const int kColumnWidth = 60;
const int kRowHeight = 20;
void MatrixScreenPrintf(int x, int y, const Matrix4x4 matrix, const char* label);
void VectorScreenPrintf(int x, int y, const Vector3& vector, const char* label);

// マウスでカメラを移動
Vector2 preMousePos{};                                                              // 前１フレームの位置
void MouseCamera(Vector3* cameraPos, Vector3* cameraRotate, char key[]);            // マウスでカメラを制御する
void MouseCameraDrawIcon(float windowWidth, float windowHeight, bool showHelpText); // マウスでカメラを制御する時アイコンを表す
#pragma endregion

#pragma region 描画関数
// 3Dスペースの平面としてネットを描画
void DrawGrid(const Matrix4x4& viewProjectionM, const Matrix4x4& viewprotM);
// 線分を描画
void DrawSegment(const Segment& segment, const Matrix4x4& viewProjectionM, const Matrix4x4& viewprotM, uint32_t color);
// 球を描画
void DrawSphere(const Sphere& sphere, const Matrix4x4& viewProjectionM, const Matrix4x4& viewprotM, uint32_t color);
// 平面を描画
void DrawPlane(const Plane& plane, const Matrix4x4& viewProjectionM, const Matrix4x4& viewprotM, uint32_t color);
#pragma endregion

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, int(kWindowWidth), int(kWindwoHeight));

	// キー入力結果を受け取る箱
	char keys[256] = {0};
	char preKeys[256] = {0};

	// 自分の変数
	Vector3 cameraPostion = {0, 1.9f, -6.49f}; // カメラの座標
	Vector3 cameraRotate = {0.26f, 0, 0};      // カメラの回転
	Vector3 gridPostion{0, 0, 0};              // ネットの座標

	Segment segment = {
	    {-0.45f, 0.41f, 0},
        {1,      0.58f, 0}
    };
	Plane plane = {
	    {0, 1, 0},
        1
    };

	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0) {
		// フレームの開始
		Novice::BeginFrame();

		// キー入力を受け取る
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		///
		/// ↓更新処理ここから
		///

		// DebugText
		ImGui::Begin("DeBug Window");
		ImGui::DragFloat3("Camera Translate", &cameraPostion.x, 0.01f);
		ImGui::DragFloat3("Camera Rotate", &cameraRotate.x, 0.01f);
		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
		ImGui::DragFloat3("Plane Normal", &plane.normal.x, 0.01f);
		plane.normal = Normalize(plane.normal);
		ImGui::DragFloat("Plane Distance", &plane.distance, 0.01f);
		ImGui::DragFloat3("Segment origin", &segment.origin.x, 0.01f);
		ImGui::DragFloat3("Segment Diff", &segment.diff.x, 0.01f);
		ImGui::End();

		// DebugCamera
		MouseCamera(&cameraPostion, &cameraRotate, keys);

		// レンダリングパイプライン計算
		if (cameraPostion.z == 0)
			cameraPostion.z = -0.0001f;
		Matrix4x4 cameraMatrix = MakeAffineMatrix({1, 1, 1}, cameraRotate, cameraPostion);
		Matrix4x4 viewMatrix = Inverse(cameraMatrix);
		Matrix4x4 viewprotMatrix = MakeViewportMatrix(0, 0, kWindowWidth, kWindwoHeight, 0, 1);
		Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, kWindowWidth / kWindwoHeight, 0.1f, 100);

		Matrix4x4 worldMatrix = MakeAffineMatrix({1, 1, 1}, {0, 0, 0}, gridPostion);
		Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));

		// 衝突判定
		uint32_t color = WHITE;
		if (IsCollision(segment, plane))
			color = RED;

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		DrawGrid(worldViewProjectionMatrix, viewprotMatrix);
		DrawPlane(plane, worldViewProjectionMatrix, viewprotMatrix, WHITE);
		DrawSegment(segment, worldViewProjectionMatrix, viewprotMatrix, color);

		MouseCameraDrawIcon(kWindowWidth, kWindwoHeight, true); // Draw DebugCamera Icon

		///
		/// ↑描画処理ここまで
		///

		// フレームの終了
		Novice::EndFrame();

		// ESCキーが押されたらループを抜ける
		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
			break;
		}
	}

	// ライブラリの終了
	Novice::Finalize();
	return 0;
}

/// --------------詳しい関数の実行する内容-------------- ///

Vector3 Add(const Vector3& v1, const Vector3& v2) { return Vector3(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z); }

Vector3 Subtract(const Vector3& v1, const Vector3& v2) { return Vector3(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z); }

Vector3 Multiply(float scalar, const Vector3& v) { return Vector3(scalar * v.x, scalar * v.y, scalar * v.z); }

Matrix4x4 MakeAffineMatrix(Vector3 scale, Vector3 rotation, Vector3 translation) {
	// Scale
	Matrix4x4 mScale = {0};
	mScale.m[0][0] = scale.x;
	mScale.m[1][1] = scale.y;
	mScale.m[2][2] = scale.z;
	mScale.m[3][3] = 1;
	// Rotation
	Matrix4x4 mRotationZ = {0};
	mRotationZ.m[0][0] = cosf(rotation.z);
	mRotationZ.m[0][1] = sinf(rotation.z);
	mRotationZ.m[1][0] = -sinf(rotation.z);
	mRotationZ.m[1][1] = cosf(rotation.z);
	mRotationZ.m[2][2] = mRotationZ.m[3][3] = 1;
	Matrix4x4 mRotationX = {0};
	mRotationX.m[1][1] = cosf(rotation.x);
	mRotationX.m[1][2] = sinf(rotation.x);
	mRotationX.m[2][1] = -sinf(rotation.x);
	mRotationX.m[2][2] = cosf(rotation.x);
	mRotationX.m[0][0] = mRotationX.m[3][3] = 1;
	Matrix4x4 mRotationY = {0};
	mRotationY.m[0][0] = cosf(rotation.y);
	mRotationY.m[2][0] = sinf(rotation.y);
	mRotationY.m[0][2] = -sinf(rotation.y);
	mRotationY.m[2][2] = cosf(rotation.y);
	mRotationY.m[1][1] = mRotationY.m[3][3] = 1;
	Matrix4x4 mRotation = Multiply(mRotationX, Multiply(mRotationY, mRotationZ));
	// Translation
	Matrix4x4 mTranslation = {0};
	mTranslation.m[0][0] = mTranslation.m[1][1] = mTranslation.m[2][2] = mTranslation.m[3][3] = 1;
	mTranslation.m[3][0] = translation.x;
	mTranslation.m[3][1] = translation.y;
	mTranslation.m[3][2] = translation.z;

	return Multiply(mScale, Multiply(mRotation, mTranslation));
}

Matrix4x4 Multiply(Matrix4x4 m1, Matrix4x4 m2) {
	Matrix4x4 result{};
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			result.m[i][j] = m1.m[i][0] * m2.m[0][j] + m1.m[i][1] * m2.m[1][j] + m1.m[i][2] * m2.m[2][j] + m1.m[i][3] * m2.m[3][j];
		}
	}
	return result;
}

Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix) {
	Vector3 result = {0};
	result.x = vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + vector.z * matrix.m[2][0] + 1.f * matrix.m[3][0];
	result.y = vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + vector.z * matrix.m[2][1] + 1.f * matrix.m[3][1];
	result.z = vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + vector.z * matrix.m[2][2] + 1.f * matrix.m[3][2];
	float w = vector.x * matrix.m[0][3] + vector.y * matrix.m[1][3] + vector.z * matrix.m[2][3] + 1.f * matrix.m[3][3];

	assert(w != 0);
	result.x /= w;
	result.y /= w;
	result.z /= w;

	return result;
}

Matrix4x4 Inverse(const Matrix4x4& m) {
	Matrix4x4 result{};
	float detA =
	    m.m[0][0] * m.m[1][1] * m.m[2][2] * m.m[3][3] + m.m[0][0] * m.m[1][2] * m.m[2][3] * m.m[3][1] + m.m[0][0] * m.m[1][3] * m.m[2][1] * m.m[3][2] - m.m[0][0] * m.m[1][3] * m.m[2][2] * m.m[3][1] -
	    m.m[0][0] * m.m[1][2] * m.m[2][1] * m.m[3][3] - m.m[0][0] * m.m[1][1] * m.m[2][3] * m.m[3][2] - m.m[0][1] * m.m[1][0] * m.m[2][2] * m.m[3][3] - m.m[0][2] * m.m[1][0] * m.m[2][3] * m.m[3][1] -
	    m.m[0][3] * m.m[1][0] * m.m[2][1] * m.m[3][2] + m.m[0][3] * m.m[1][0] * m.m[2][2] * m.m[3][1] + m.m[0][2] * m.m[1][0] * m.m[2][1] * m.m[3][3] + m.m[0][1] * m.m[1][0] * m.m[2][3] * m.m[3][2] +
	    m.m[0][1] * m.m[1][2] * m.m[2][0] * m.m[3][3] + m.m[0][2] * m.m[1][3] * m.m[2][0] * m.m[3][1] + m.m[0][3] * m.m[1][1] * m.m[2][0] * m.m[3][2] - m.m[0][3] * m.m[1][2] * m.m[2][0] * m.m[3][1] -
	    m.m[0][2] * m.m[1][1] * m.m[2][0] * m.m[3][3] - m.m[0][1] * m.m[1][3] * m.m[2][0] * m.m[3][2] - m.m[0][1] * m.m[1][2] * m.m[2][3] * m.m[3][0] - m.m[0][2] * m.m[1][3] * m.m[2][1] * m.m[3][0] -
	    m.m[0][3] * m.m[1][1] * m.m[2][2] * m.m[3][0] + m.m[0][3] * m.m[1][2] * m.m[2][1] * m.m[3][0] + m.m[0][2] * m.m[1][1] * m.m[2][3] * m.m[3][0] + m.m[0][1] * m.m[1][3] * m.m[2][2] * m.m[3][0];

	float adjA[4][4] = {0};
	adjA[0][0] = m.m[1][1] * m.m[2][2] * m.m[3][3] + m.m[1][2] * m.m[2][3] * m.m[3][1] + m.m[1][3] * m.m[2][1] * m.m[3][2] - m.m[1][3] * m.m[2][2] * m.m[3][1] - m.m[1][2] * m.m[2][1] * m.m[3][3] -
	             m.m[1][1] * m.m[2][3] * m.m[3][2];
	adjA[0][1] = -m.m[0][1] * m.m[2][2] * m.m[3][3] - m.m[0][2] * m.m[2][3] * m.m[3][1] - m.m[0][3] * m.m[2][1] * m.m[3][2] + m.m[0][3] * m.m[2][2] * m.m[3][1] + m.m[0][2] * m.m[2][1] * m.m[3][3] +
	             m.m[0][1] * m.m[2][3] * m.m[3][2];
	adjA[0][2] = m.m[0][1] * m.m[1][2] * m.m[3][3] + m.m[0][2] * m.m[1][3] * m.m[3][1] + m.m[0][3] * m.m[1][1] * m.m[3][2] - m.m[0][3] * m.m[1][2] * m.m[3][1] - m.m[0][2] * m.m[1][1] * m.m[3][3] -
	             m.m[0][1] * m.m[1][3] * m.m[3][2];
	adjA[0][3] = -m.m[0][1] * m.m[1][2] * m.m[2][3] - m.m[0][2] * m.m[1][3] * m.m[2][1] - m.m[0][3] * m.m[1][1] * m.m[2][2] + m.m[0][3] * m.m[1][2] * m.m[2][1] + m.m[0][2] * m.m[1][1] * m.m[2][3] +
	             m.m[0][1] * m.m[1][3] * m.m[2][2];
	adjA[1][0] = -m.m[1][0] * m.m[2][2] * m.m[3][3] - m.m[1][2] * m.m[2][3] * m.m[3][0] - m.m[1][3] * m.m[2][0] * m.m[3][2] + m.m[1][3] * m.m[2][2] * m.m[3][0] + m.m[1][2] * m.m[2][0] * m.m[3][3] +
	             m.m[1][0] * m.m[2][3] * m.m[3][2];
	adjA[1][1] = m.m[0][0] * m.m[2][2] * m.m[3][3] + m.m[0][2] * m.m[2][3] * m.m[3][0] + m.m[0][3] * m.m[2][0] * m.m[3][2] - m.m[0][3] * m.m[2][2] * m.m[3][0] - m.m[0][2] * m.m[2][0] * m.m[3][3] -
	             m.m[0][0] * m.m[2][3] * m.m[3][2];
	adjA[1][2] = -m.m[0][0] * m.m[1][2] * m.m[3][3] - m.m[0][2] * m.m[1][3] * m.m[3][0] - m.m[0][3] * m.m[1][0] * m.m[3][2] + m.m[0][3] * m.m[1][2] * m.m[3][0] + m.m[0][2] * m.m[1][0] * m.m[3][3] +
	             m.m[0][0] * m.m[1][3] * m.m[3][2];
	adjA[1][3] = m.m[0][0] * m.m[1][2] * m.m[2][3] + m.m[0][2] * m.m[1][3] * m.m[2][0] + m.m[0][3] * m.m[1][0] * m.m[2][2] - m.m[0][3] * m.m[1][2] * m.m[2][0] - m.m[0][2] * m.m[1][0] * m.m[2][3] -
	             m.m[0][0] * m.m[1][3] * m.m[2][2];
	adjA[2][0] = m.m[1][0] * m.m[2][1] * m.m[3][3] + m.m[1][1] * m.m[2][3] * m.m[3][0] + m.m[1][3] * m.m[2][0] * m.m[3][1] - m.m[1][3] * m.m[2][1] * m.m[3][0] - m.m[1][1] * m.m[2][0] * m.m[3][3] -
	             m.m[1][0] * m.m[2][3] * m.m[3][1];
	adjA[2][1] = -m.m[0][0] * m.m[2][1] * m.m[3][3] - m.m[0][1] * m.m[2][3] * m.m[3][0] - m.m[0][3] * m.m[2][0] * m.m[3][1] + m.m[0][3] * m.m[2][1] * m.m[3][0] + m.m[0][1] * m.m[2][0] * m.m[3][3] +
	             m.m[0][0] * m.m[2][3] * m.m[3][1];
	adjA[2][2] = m.m[0][0] * m.m[1][1] * m.m[3][3] + m.m[0][1] * m.m[1][3] * m.m[3][0] + m.m[0][3] * m.m[1][0] * m.m[3][1] - m.m[0][3] * m.m[1][1] * m.m[3][0] - m.m[0][1] * m.m[1][0] * m.m[3][3] -
	             m.m[0][0] * m.m[1][3] * m.m[3][1];
	adjA[2][3] = -m.m[0][0] * m.m[1][1] * m.m[2][3] - m.m[0][1] * m.m[1][3] * m.m[2][0] - m.m[0][3] * m.m[1][0] * m.m[2][1] + m.m[0][3] * m.m[1][1] * m.m[2][0] + m.m[0][1] * m.m[1][0] * m.m[2][3] +
	             m.m[0][0] * m.m[1][3] * m.m[2][1];
	adjA[3][0] = -m.m[1][0] * m.m[2][1] * m.m[3][2] - m.m[1][1] * m.m[2][2] * m.m[3][0] - m.m[1][2] * m.m[2][0] * m.m[3][1] + m.m[1][2] * m.m[2][1] * m.m[3][0] + m.m[1][1] * m.m[2][0] * m.m[3][2] +
	             m.m[1][0] * m.m[2][2] * m.m[3][1];
	adjA[3][1] = m.m[0][0] * m.m[2][1] * m.m[3][2] + m.m[0][1] * m.m[2][2] * m.m[3][0] + m.m[0][2] * m.m[2][0] * m.m[3][1] - m.m[0][2] * m.m[2][1] * m.m[3][0] - m.m[0][1] * m.m[2][0] * m.m[3][2] -
	             m.m[0][0] * m.m[2][2] * m.m[3][1];
	adjA[3][2] = -m.m[0][0] * m.m[1][1] * m.m[3][2] - m.m[0][1] * m.m[1][2] * m.m[3][0] - m.m[0][2] * m.m[1][0] * m.m[3][1] + m.m[0][2] * m.m[1][1] * m.m[3][0] + m.m[0][1] * m.m[1][0] * m.m[3][2] +
	             m.m[0][0] * m.m[1][2] * m.m[3][1];
	adjA[3][3] = m.m[0][0] * m.m[1][1] * m.m[2][2] + m.m[0][1] * m.m[1][2] * m.m[2][0] + m.m[0][2] * m.m[1][0] * m.m[2][1] - m.m[0][2] * m.m[1][1] * m.m[2][0] - m.m[0][1] * m.m[1][0] * m.m[2][2] -
	             m.m[0][0] * m.m[1][2] * m.m[2][1];

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			result.m[i][j] = 1 / detA * adjA[i][j];
		}
	}
	return result;
}

Matrix4x4 Transpose(const Matrix4x4& m) {
	Matrix4x4 result{};
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			result.m[i][j] = m.m[j][i];
		}
	}
	return result;
}

Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip) {
	Matrix4x4 result = {0};

	result.m[0][0] = 1 / aspectRatio * cosf(fovY / 2) / sinf(fovY / 2);
	result.m[1][1] = cosf(fovY / 2) / sinf(fovY / 2);
	result.m[2][2] = farClip / (farClip - nearClip);
	result.m[3][2] = (-nearClip * farClip) / (farClip - nearClip);
	result.m[2][3] = 1;
	return result;
}

Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip) {
	Matrix4x4 result = {0};
	result.m[0][0] = 2 / (right - left);
	result.m[1][1] = 2 / (top - bottom);
	result.m[2][2] = 1 / (farClip - nearClip);
	result.m[3][0] = (left + right) / (left - right);
	result.m[3][1] = (top + bottom) / (bottom - top);
	result.m[3][2] = nearClip / (nearClip - farClip);
	result.m[3][3] = 1;
	return result;
}

Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth) {
	Matrix4x4 result = {0};
	result.m[0][0] = width / 2;
	result.m[1][1] = -height / 2;
	result.m[2][2] = maxDepth - minDepth;
	result.m[3][0] = left + width / 2;
	result.m[3][1] = top + height / 2;
	result.m[3][2] = minDepth;
	result.m[3][3] = 1;
	return result;
}

Vector3 Cross(const Vector3& v1, const Vector3& v2) {
	Vector3 result{};
	result.x = v1.y * v2.z - v1.z * v2.y;
	result.y = v1.z * v2.x - v1.x * v2.z;
	result.z = v1.x * v2.y - v1.y * v2.x;
	return result;
}

float Length(const Vector3& v) { return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z); }

Vector3 Normalize(const Vector3& v) {
	float length = sqrtf(powf(v.x, 2) + powf(v.y, 2) + powf(v.z, 2));
	return Vector3(v.x / length, v.y / length, v.z / length);
}

float Dot(const Vector3& v1, const Vector3& v2) { return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z; }

void MatrixScreenPrintf(int x, int y, const Matrix4x4 matrix, const char* label) {
	Novice::ScreenPrintf(x, y, label);
	for (int row = 0; row < 4; ++row) {
		for (int column = 0; column < 4; ++column) {
			Novice::ScreenPrintf(x + column * kColumnWidth, y + row * kRowHeight + kRowHeight, "%6.02f", matrix.m[row][column]);
		}
	}
}

void VectorScreenPrintf(int x, int y, const Vector3& vector, const char* label) {
	Novice::ScreenPrintf(x, y, "%.02f", vector.x);
	Novice::ScreenPrintf(x + kColumnWidth, y, "%.02f", vector.y);
	Novice::ScreenPrintf(x + kColumnWidth * 2, y, "%.02f", vector.z);
	Novice::ScreenPrintf(x + kColumnWidth * 3, y, "%s", label);
}

void MouseCamera(Vector3* cameraPos, Vector3* cameraRotate, char key[]) {
	float moveSpeed = 0.1f;     // キーボードで移動のスピード
	float wheelSpeed = 0.3f;    // マウスのホイールスクロールのスピード
	float rotationSpeed = 0.4f; // マウスの右キーで回るスピード
	float dragSpeed = 0.5f;     // マウスの中キーで移動のスピード

	Vector3 front{}, right{}, up{}, move{}; // カメラの前・横・上の向きと総合の移動ベクトル
	bool isMouseMove = false;               // マウスで移動
	//  カメラの前方向を計算
	front.x = sinf(cameraRotate->y) * cosf(cameraRotate->x);
	front.y = -sinf(cameraRotate->x);
	front.z = cosf(cameraRotate->y) * cosf(cameraRotate->x);
	front = Normalize(front);
	// カメラの横方向を計算
	Vector3 worldUp{0, 1, 0};
	right = Cross(front, worldUp);
	right = Normalize(right);
	// カメラの上方向を計算
	up = Cross(right, front);
	up = Normalize(up);

	if (Novice::IsPressMouse(1)) {
		// キーボードで移動
		if (key[DIK_W]) {
			move = Add(move, front);
		} else if (key[DIK_S]) {
			move = Add(move, Multiply(-1, front));
		}
		if (key[DIK_D]) {
			move = Add(move, Multiply(-1, right));
		} else if (key[DIK_A]) {
			move = Add(move, right);
		}
	} else {
		// マウスのホイールスクロール
		isMouseMove = true;
		move = Multiply(float(Novice::GetWheel()) * wheelSpeed * 0.01f, front);
	}

	// カメラをマウスで回転
	int mousePosX, mousePosY;
	Novice::GetMousePosition(&mousePosX, &mousePosY);
	Vector2 currentMousePos{};
	if (Novice::IsPressMouse(1)) {
		// マウスの右キー
		currentMousePos = {float(mousePosX), float(mousePosY)};
		cameraRotate->x += (currentMousePos.y - preMousePos.y) * rotationSpeed * 0.01f;
		cameraRotate->y += (currentMousePos.x - preMousePos.x) * rotationSpeed * 0.01f;
		preMousePos = {float(mousePosX), float(mousePosY)};
	} else if (Novice::IsPressMouse(2)) {
		// マウスの中キー
		isMouseMove = true;
		currentMousePos = {float(mousePosX), float(mousePosY)};
		Vector3 mouseVector = {currentMousePos.x - preMousePos.x, currentMousePos.y - preMousePos.y, 0};
		if (abs(mouseVector.x) > 1 || abs(mouseVector.y) > 1) {
			move = Add(move, Multiply(mouseVector.x * dragSpeed * 0.01f, right));
			move = Add(move, Multiply(mouseVector.y * dragSpeed * 0.01f, up));
		}
		preMousePos = {float(mousePosX), float(mousePosY)};
	} else {
		preMousePos = {float(mousePosX), float(mousePosY)};
	}

	// 正規化、速度は同じにするために
	if (!isMouseMove) {
		if (move.x != 0 || move.y != 0 || move.z != 0) {
			move = Normalize(move);
			move = Multiply(moveSpeed, move);
		}
	}
	cameraPos->x += move.x;
	cameraPos->y += move.y;
	cameraPos->z += move.z;
}

void MouseCameraDrawIcon(float windowWidth, float windowHeight, bool showHelpText) {
	windowWidth;
	if (showHelpText) {
		// Text
		float textLine = 17;
		Vector2 textPos{0 + 5, windowHeight - textLine * 5 - 5};
		Novice::ScreenPrintf(int(textPos.x), int(textPos.y), "----- Mouse Camera Usage -----");
		Novice::ScreenPrintf(int(textPos.x), int(textPos.y + textLine * 1), "(The control logic is the same as Unity)");
		Novice::ScreenPrintf(int(textPos.x), int(textPos.y + textLine * 2), "Right mouse: camera rotation");
		Novice::ScreenPrintf(int(textPos.x), int(textPos.y + textLine * 3), "Middle mouse: camera movement and zoom in/out");
		Novice::ScreenPrintf(int(textPos.x), int(textPos.y + textLine * 4), "Right mouse + WASD: camera move");
	}
	// Icon
	int mouseX, mouseY;
	Novice::GetMousePosition(&mouseX, &mouseY);
	Vector2 currentMousePos{float(mouseX), float(mouseY)};
	Vector2 eyeSize{9, 5}, boxSize{3, 3}, boxPos{9, 7}; // マウスの右キーのアイコン
	Vector2 headSize{6, 7}, fingerSize{3, 9};           // マウスの中キーのアイコン
	if (Novice::IsPressMouse(1)) {
		// Eye
		Novice::DrawEllipse((int)currentMousePos.x, (int)currentMousePos.y, int(eyeSize.x), int(eyeSize.y), 0, WHITE, kFillModeSolid);
		Novice::DrawEllipse((int)currentMousePos.x, (int)currentMousePos.y, int(eyeSize.x + 1), int(eyeSize.y + 1), 0, BLACK, kFillModeWireFrame);
		Novice::DrawEllipse((int)currentMousePos.x, (int)currentMousePos.y, int(eyeSize.y - 1), int(eyeSize.y - 1), 0, BLACK, kFillModeWireFrame);
		// MoveBox
		Novice::DrawBox(int(currentMousePos.x + boxPos.x), int(currentMousePos.y + boxPos.y), int(boxSize.x), int(boxSize.y), 0, WHITE, kFillModeSolid);
		Novice::DrawBox(int(currentMousePos.x + boxPos.x), int(currentMousePos.y + boxPos.y), int(boxSize.x + 1), int(boxSize.y + 1), 0, BLACK, kFillModeWireFrame);
		Novice::DrawBox(int(currentMousePos.x + boxPos.x), int(currentMousePos.y + boxPos.y + boxSize.y + 1), int(boxSize.x), int(boxSize.y), 0, WHITE, kFillModeSolid);
		Novice::DrawBox(int(currentMousePos.x + boxPos.x), int(currentMousePos.y + boxPos.y + boxSize.y + 1), int(boxSize.x + 1), int(boxSize.y + 1), 0, BLACK, kFillModeWireFrame);
		Novice::DrawBox(int(currentMousePos.x + boxPos.x - boxSize.x - 1), int(currentMousePos.y + boxPos.y + boxSize.y + 1), int(boxSize.x), int(boxSize.y), 0, WHITE, kFillModeSolid);
		Novice::DrawBox(int(currentMousePos.x + boxPos.x - boxSize.x - 1), int(currentMousePos.y + boxPos.y + boxSize.y + 1), int(boxSize.x + 1), int(boxSize.y + 1), 0, BLACK, kFillModeWireFrame);
		Novice::DrawBox(int(currentMousePos.x + boxPos.x + boxSize.x + 1), int(currentMousePos.y + boxPos.y + boxSize.y + 1), int(boxSize.x), int(boxSize.y), 0, WHITE, kFillModeSolid);
		Novice::DrawBox(int(currentMousePos.x + boxPos.x + boxSize.x + 1), int(currentMousePos.y + boxPos.y + boxSize.y + 1), int(boxSize.x + 1), int(boxSize.y + 1), 0, BLACK, kFillModeWireFrame);
	} else if (Novice::IsPressMouse(2)) {
		// Finger
		Novice::DrawBox(int(currentMousePos.x - 5), int(currentMousePos.y - 13), int(fingerSize.x), int(fingerSize.y), 0, WHITE, kFillModeSolid);
		Novice::DrawBox(int(currentMousePos.x - 5), int(currentMousePos.y - 13), int(fingerSize.x + 1), int(fingerSize.y + 1), 0, BLACK, kFillModeWireFrame);
		Novice::DrawBox(int(currentMousePos.x - 8), int(currentMousePos.y - 7), int(fingerSize.x), int(fingerSize.y), 0, WHITE, kFillModeSolid);
		Novice::DrawBox(int(currentMousePos.x - 8), int(currentMousePos.y - 7), int(fingerSize.x + 1), int(fingerSize.y + 1), 0, BLACK, kFillModeWireFrame);
		Novice::DrawBox(int(currentMousePos.x - 1), int(currentMousePos.y - 14), int(fingerSize.x), int(fingerSize.y), 0, WHITE, kFillModeSolid);
		Novice::DrawBox(int(currentMousePos.x - 1), int(currentMousePos.y - 14), int(fingerSize.x + 1), int(fingerSize.y + 1), 0, BLACK, kFillModeWireFrame);
		Novice::DrawBox(int(currentMousePos.x + 6), int(currentMousePos.y - 10), int(fingerSize.x), int(fingerSize.y), 0, WHITE, kFillModeSolid);
		Novice::DrawBox(int(currentMousePos.x + 6), int(currentMousePos.y - 10), int(fingerSize.x + 1), int(fingerSize.y + 1), 0, BLACK, kFillModeWireFrame);
		Novice::DrawBox(int(currentMousePos.x + 3), int(currentMousePos.y - 12), int(fingerSize.x), int(fingerSize.y), 0, WHITE, kFillModeSolid);
		Novice::DrawBox(int(currentMousePos.x + 3), int(currentMousePos.y - 12), int(fingerSize.x + 1), int(fingerSize.y + 1), 0, BLACK, kFillModeWireFrame);
		// Head
		Novice::DrawEllipse((int)currentMousePos.x + 2, (int)currentMousePos.y + 1, int(headSize.x), int(headSize.y), 0, WHITE, kFillModeSolid);
		Novice::DrawEllipse((int)currentMousePos.x + 2, (int)currentMousePos.y + 1, int(headSize.x + 1), int(headSize.y + 1), 0, BLACK, kFillModeWireFrame);
	}
}

void DrawGrid(const Matrix4x4& viewProjectionM, const Matrix4x4& viewprotM) {
	const float kGridHalfWidth = 2.f;                                    // Girdの半分の幅
	const uint32_t kSubdivision = 10;                                    // 分割数
	const float kGridEvery = (kGridHalfWidth * 2) / float(kSubdivision); // 一つ分の長さ

	// 線の座標を記録する
	Vector3 horizontalStartPos_local[kSubdivision + 1]{};
	Vector3 horizontalEndPos_local[kSubdivision + 1]{};
	Vector3 verticalStartPos_local[kSubdivision + 1]{};
	Vector3 verticalEndPos_local[kSubdivision + 1]{};

	Vector3 horizontalStartPos_screen[kSubdivision + 1]{};
	Vector3 horizontalEndPos_screen[kSubdivision + 1]{};
	Vector3 verticalStartPos_screen[kSubdivision + 1]{};
	Vector3 verticalEndPos_screen[kSubdivision + 1]{};

	// 奥から手前への線を順々に引いていく
	for (uint32_t xIndex = 0; xIndex <= kSubdivision; xIndex++) {
		// ローカル座標を計算する
		horizontalStartPos_local[xIndex].x = -kGridHalfWidth;
		horizontalEndPos_local[xIndex].x = kGridHalfWidth;
		horizontalStartPos_local[xIndex].z = kGridHalfWidth - xIndex * kGridEvery;
		horizontalEndPos_local[xIndex].z = kGridHalfWidth - xIndex * kGridEvery;
		// スクリーン座標に変換する
		Vector3 ndcStart = Transform(horizontalStartPos_local[xIndex], viewProjectionM);
		horizontalStartPos_screen[xIndex] = Transform(ndcStart, viewprotM);
		Vector3 ndcEnd = Transform(horizontalEndPos_local[xIndex], viewProjectionM);
		horizontalEndPos_screen[xIndex] = Transform(ndcEnd, viewprotM);
		// 描画
		if (xIndex != 5)
			Novice::DrawLine(
			    int(horizontalStartPos_screen[xIndex].x), int(horizontalStartPos_screen[xIndex].y), int(horizontalEndPos_screen[xIndex].x), int(horizontalEndPos_screen[xIndex].y), 0xAAAAAAFF);
		else
			Novice::DrawLine(int(horizontalStartPos_screen[xIndex].x), int(horizontalStartPos_screen[xIndex].y), int(horizontalEndPos_screen[xIndex].x), int(horizontalEndPos_screen[xIndex].y), BLACK);
	}
	// 左から右への線を順々に引いていく
	for (uint32_t zIndex = 0; zIndex <= kSubdivision; zIndex++) {
		// ローカル座標を計算する
		verticalStartPos_local[zIndex].x = kGridHalfWidth - zIndex * kGridEvery;
		verticalEndPos_local[zIndex].x = kGridHalfWidth - zIndex * kGridEvery;
		verticalStartPos_local[zIndex].z = -kGridHalfWidth;
		verticalEndPos_local[zIndex].z = kGridHalfWidth;
		// スクリーン座標に変換する
		Vector3 ndcStart = Transform(verticalStartPos_local[zIndex], viewProjectionM);
		verticalStartPos_screen[zIndex] = Transform(ndcStart, viewprotM);
		Vector3 ndcEnd = Transform(verticalEndPos_local[zIndex], viewProjectionM);
		verticalEndPos_screen[zIndex] = Transform(ndcEnd, viewprotM);
		// 描画
		if (zIndex != 5)
			Novice::DrawLine(int(verticalStartPos_screen[zIndex].x), int(verticalStartPos_screen[zIndex].y), int(verticalEndPos_screen[zIndex].x), int(verticalEndPos_screen[zIndex].y), 0xAAAAAAFF);
		else
			Novice::DrawLine(int(verticalStartPos_screen[zIndex].x), int(verticalStartPos_screen[zIndex].y), int(verticalEndPos_screen[zIndex].x), int(verticalEndPos_screen[zIndex].y), BLACK);
	}
}

void DrawSegment(const Segment& segment, const Matrix4x4& viewProjectionM, const Matrix4x4& viewprotM, uint32_t color) {
	Vector3 start = Transform(Transform(segment.origin, viewProjectionM), viewprotM);
	Vector3 end = Transform(Transform(Add(segment.origin, segment.diff), viewProjectionM), viewprotM);
	Novice::DrawLine((int)start.x, (int)start.y, (int)end.x, (int)end.y, color);
}

void DrawSphere(const Sphere& sphere, const Matrix4x4& viewProjectionM, const Matrix4x4& viewprotM, uint32_t color) {
	const uint32_t kSubdivison = 20;                            // 分割数
	const float kLatEvery = acosf(-1) / float(kSubdivison);     // 緯度分割１つ分の角度
	const float kLonEvery = 2 * acosf(-1) / float(kSubdivison); // 経度分割１つ分の角度
	// 緯度の方向に分割　-pi/2 ~ pi/2
	for (uint32_t latIndex = 0; latIndex < kSubdivison; latIndex++) {
		float lat = -acosf(-1) / 2 + kLatEvery * latIndex; // 現在の緯度
		// 経度の方向に分割　0 ~ 2p	i
		for (uint32_t lonIndex = 0; lonIndex < kSubdivison; lonIndex++) {
			float lon = lonIndex * kLonEvery; // 現在の経度
			// 描画用のローカル座標 Vector abc
			Vector3 a{}, b{}, c{};
			a.x = sphere.center.x + sphere.radius * cosf(lat) * cosf(lon);
			a.y = sphere.center.y + sphere.radius * sinf(lat);
			a.z = sphere.center.z + sphere.radius * cosf(lat) * sinf(lon);
			b.x = sphere.center.x + sphere.radius * cosf(lat + kLatEvery) * cosf(lon);
			b.y = sphere.center.y + sphere.radius * sinf(lat + kLatEvery);
			b.z = sphere.center.z + sphere.radius * cosf(lat + kLatEvery) * sinf(lon);
			c.x = sphere.center.x + sphere.radius * cosf(lat) * cosf(lon + kLonEvery);
			c.y = sphere.center.y + sphere.radius * sinf(lat);
			c.z = sphere.center.z + sphere.radius * cosf(lat) * sinf(lon + kLonEvery);
			// スクリーン座標に変換する
			Vector3 ndcA = Transform(a, viewProjectionM);
			Vector3 screenA = Transform(ndcA, viewprotM);
			Vector3 ndcB = Transform(b, viewProjectionM);
			Vector3 screenB = Transform(ndcB, viewprotM);
			Vector3 ndcC = Transform(c, viewProjectionM);
			Vector3 screenC = Transform(ndcC, viewprotM);
			// 描画
			Novice::DrawLine(int(screenA.x), int(screenA.y), int(screenB.x), int(screenB.y), color);
			Novice::DrawLine(int(screenA.x), int(screenA.y), int(screenC.x), int(screenC.y), color);
		}
	}
}

void DrawPlane(const Plane& plane, const Matrix4x4& viewProjectionM, const Matrix4x4& viewprotM, uint32_t color) {
	// 法線は必ず単位ベクトルにする
	Vector3 planeNormal_normalize = plane.normal;
	// 中心点を決める
	Vector3 center = Multiply(plane.distance, planeNormal_normalize);
	// 中心点によって垂直してる4つのベクトル
	Vector3 perpendiculars[4]{};
	// 法線と垂直ベクトルを一つを求める
	perpendiculars[0] = Normalize(Perpendicular(planeNormal_normalize));
	// 2の逆ベクトルを求める
	perpendiculars[1] = {-perpendiculars[0].x, -perpendiculars[0].y, -perpendiculars[0].z};
	// 2と法線のクロス積を求める
	perpendiculars[2] = Cross(planeNormal_normalize, perpendiculars[0]);
	// 4の逆ベクトルを求める
	perpendiculars[3] = {-perpendiculars[2].x, -perpendiculars[2].y, -perpendiculars[2].z};
	// 4頂点
	// 4垂直ベクトルを中心点にそれぞれ定数倍して足すと頂点が出来上がる
	Vector3 points[4]{};
	for (int i = 0; i < 4; ++i) {
		Vector3 extend = Multiply(2, perpendiculars[i]);
		Vector3 point = Add(center, extend);
		points[i] = Transform(Transform(point, viewProjectionM), viewprotM);
	}
	// 4頂点を一つずつにつながて、描画する
	Novice::DrawLine(int(points[0].x), int(points[0].y), int(points[2].x), int(points[2].y), color);
	Novice::DrawLine(int(points[1].x), int(points[1].y), int(points[2].x), int(points[2].y), color);
	Novice::DrawLine(int(points[1].x), int(points[1].y), int(points[3].x), int(points[3].y), color);
	Novice::DrawLine(int(points[3].x), int(points[3].y), int(points[0].x), int(points[0].y), color);
	// Novice::DrawTriangle(int(points[0].x), int(points[0].y), int(points[2].x), int(points[2].y), int(points[1].x), int(points[1].y), color, kFillModeSolid);
	//  Novice::DrawTriangle(int(points[1].x), int(points[1].y), int(points[3].x), int(points[3].y), int(points[0].x), int(points[0].y), color, kFillModeSolid);
}

Vector3 Project(const Vector3& v1, const Vector3& v2) {
	float dot = Dot(v1, Normalize(v2));
	return Vector3(dot * Normalize(v2).x, dot * Normalize(v2).y, dot * Normalize(v2).z);
}

Vector3 ClosestPoint(const Vector3& point, const Segment& segment) {
	Vector3 a = Subtract(point, segment.origin);
	Vector3 b = segment.diff;
	Vector3 projBA = Project(a, b);
	return Add(segment.origin, projBA);
}

Vector3 Perpendicular(const Vector3& vector) {
	if (vector.x != 0 || vector.y != 0)
		return {-vector.y, vector.x, 0};
	return {0, -vector.z, vector.x};
}

bool IsCollision(const Sphere& s1, const Sphere& s2) {
	float distance = Length(Subtract(s2.center, s1.center));
	if (distance <= s1.radius + s2.radius)
		return true;
	return false;
}

bool IsCollision(const Sphere& sphere, const Plane& plane) {
	Vector3 planeNormal_normalize = Normalize(plane.normal);
	float k = fabsf(Dot(planeNormal_normalize, sphere.center) - plane.distance);
	if (k <= sphere.radius)
		return true;
	return false;
}

bool IsCollision(const Segment& segment, const Plane& plane) {
	Vector3 planeNormal_normalize = Normalize(plane.normal);
	float dot = Dot(planeNormal_normalize, segment.diff);
	// 垂直の場合
	if (dot == 0.0f)
		return false;
	// もし0<=t<=1なら、衝突している点は線分の一つの点
	float t = (plane.distance - Dot(segment.origin, planeNormal_normalize)) / dot;
	if (t >= 0 && t <= 1)
		return true;
	return false;
}
