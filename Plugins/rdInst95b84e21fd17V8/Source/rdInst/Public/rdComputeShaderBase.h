#pragma once
/*
#include "GlobalShader.h"
#include "UniformBuffer.h"
#include "RHICommandList.h"
//#include "DynamicRHIResourceArray.h"

class FComputeShaderDeclaration : public FGlobalShader {

	DECLARE_SHADER_TYPE(FComputeShaderDeclaration,Global);
public:
	FComputeShaderDeclaration() {}

	explicit FComputeShaderDeclaration(const ShaderMetaType::CompiledShaderInitializerType& initType);
#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>4
	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& parms) {
		return GetMaxSupportedFeatureLevel(parms.Platform)>=ERHIFeatureLevel::SM5;
	};
#endif
	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& parms,FShaderCompilerEnvironment& outEnvironment);
private:
	//This is the actual output resource that we will bind to the compute shader
	FShaderResourceParameter OutputTexture;
	FShaderResourceParameter OutputColorTexture;
	FShaderResourceParameter PointPosData;
	FShaderResourceParameter PointPosDataBuffer;
	FShaderResourceParameter PointColorData;
	FShaderResourceParameter PointColorDataBuffer;
};


class FComputeShaderTransposeDeclaration : public FGlobalShader {

	DECLARE_SHADER_TYPE(FComputeShaderTransposeDeclaration,Global);
public:
	FComputeShaderTransposeDeclaration() {}

	explicit FComputeShaderTransposeDeclaration(const ShaderMetaType::CompiledShaderInitializerType& Initializer);

#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>4
	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters) {
		return GetMaxSupportedFeatureLevel(Parameters.Platform) >= ERHIFeatureLevel::SM5;
	};
#endif

	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters,FShaderCompilerEnvironment& OutEnvironment);

	// This function is required to bind our constant / uniform buffers to the shader.
//	void SetUniformBuffers(FRHICommandList& RHICmdList, FComputeShaderConstantParameters& ConstantParameters, FComputeShaderVariableParameters& VariableParameters);
	// This is used to clean up the buffer binds after each invocation to let them be changed and used elsewhere if needed.
	void UnbindBuffers(FRHICommandList& RHICmdList);

private:
	// This is the actual output resource that we will bind to the compute shader
	FShaderResourceParameter PointPosData;
	FShaderResourceParameter PointPosDataBuffer;
	FShaderResourceParameter PointColorData;
	FShaderResourceParameter PointColorDataBuffer;
};

class FComputeShaderModule : public IModuleInterface {

	void StartupModule() override {
		FString ShaderDirectory=FPaths::Combine(FPaths::ProjectPluginsDir(),TEXT("Shaders"));
		//AddShaderSourceDirectoryMapping("/rdComputeShader",ShaderDirectory);
	}
};
*/