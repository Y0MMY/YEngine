//#include "TexturePreprocessor.h"
//#include <filesystem>
//#include <sstream>
//#include "Mesh.h"
//#include <YMath.h>
//
//
//void TexturePreprocessor::ValidateNormalMap(const std::string& pathIn, float thresholdMin, float thresholdMax)
//{
//	OutputDebugStringA(("Validating normal map [" + pathIn + "]\n").c_str());
//	// function for processing each normal in texture
//	using namespace DirectX;
//	auto sum = XMVectorZero();
//	const auto ProcessNormal = [thresholdMin, thresholdMax, &sum](FXMVECTOR n, int x, int y) -> XMVECTOR
//	{
//		const float len = XMVectorGetX(XMVector3Length(n));
//		const float z = XMVectorGetZ(n);
//		if (len < thresholdMin || len > thresholdMax)
//		{
//			XMFLOAT3 vec;
//			XMStoreFloat3(&vec, n);
//			std::ostringstream oss;
//			oss << "Bad normal length: " << len << " at: (" << x << "," << y << ") normal: (" << vec.x << "," << vec.y << "," << vec.z << ")\n";
//			OutputDebugStringA(oss.str().c_str());
//		}
//		if (z < 0.0f)
//		{
//			XMFLOAT3 vec;
//			XMStoreFloat3(&vec, n);
//			std::ostringstream oss;
//			oss << "Bad normal Z direction at: (" << x << "," << y << ") normal: (" << vec.x << "," << vec.y << "," << vec.z << ")\n";
//			OutputDebugStringA(oss.str().c_str());
//		}
//		sum = XMVectorAdd(sum, n);
//		return n;
//	};
//	// output bias
//	{
//		XMFLOAT2 sumv;
//		XMStoreFloat2(&sumv, sum);
//		std::ostringstream oss;
//		oss << "Normal map biases: (" << sumv.x << "," << sumv.y << ")\n";
//		OutputDebugStringA(oss.str().c_str());
//	}
//}
//
