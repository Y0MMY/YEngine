#pragma once
#include <DirectXMath.h>
#include <vector>
#include <string>

DirectX::XMFLOAT3 ExtractEulerAngles( const DirectX::XMFLOAT4X4& matrix );

DirectX::XMFLOAT3 ExtractTranslation( const DirectX::XMFLOAT4X4& matrix ); 

std::vector<std::string> TokenizeQuoted(const std::string& input);

std::wstring ToWide(const std::string& narrow);

std::string ToNarrow(const std::wstring& wide);