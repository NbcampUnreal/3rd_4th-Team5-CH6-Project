//
// rdComputeShaderBase.cpp
//
// Copyright (c) 2025 Recourse Design ltd. All rights reserved.
//
// Creation Date: 1st February 2025
// Last Modified: 5th April 2025
//
/*
#include "rdComputeShaderBase.h"
#include "ShaderParameterUtils.h"
#include "RHIStaticStates.h"

//----------------------------------------------------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------------------------------------------------
FComputeShaderDeclaration::FComputeShaderDeclaration(const ShaderMetaType::CompiledShaderInitializerType& Initializer) : FGlobalShader(Initializer) {
}

//----------------------------------------------------------------------------------------------------------------
// ModifyCompilationEnvironment
//----------------------------------------------------------------------------------------------------------------
void FComputeShaderDeclaration::ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment) {
	FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
	OutEnvironment.CompilerFlags.Add(CFLAG_StandardOptimization);
}



//----------------------------------------------------------------------------------------------------------------
// FComputeShaderTransposeDeclaration
//----------------------------------------------------------------------------------------------------------------
FComputeShaderTransposeDeclaration::FComputeShaderTransposeDeclaration(const ShaderMetaType::CompiledShaderInitializerType& Initializer) : FGlobalShader(Initializer) {
	//This call is what lets the shader system know that the surface OutputTexture is going to be available in the shader. The second parameter is the name it will be known by in the shader
	PointPosData.Bind(Initializer.ParameterMap, TEXT("PointPosData"));
	PointPosDataBuffer.Bind(Initializer.ParameterMap, TEXT("PointPosDataBuffer"));
}

//----------------------------------------------------------------------------------------------------------------
// ModifyCompilationEnvironment
//----------------------------------------------------------------------------------------------------------------
void FComputeShaderTransposeDeclaration::ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment) {
	FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
	OutEnvironment.CompilerFlags.Add(CFLAG_StandardOptimization);
}

//----------------------------------------------------------------------------------------------------------------

IMPLEMENT_SHADER_TYPE(,FComputeShaderDeclaration,TEXT("/rdComputeShader/rdMeshlets.usf"),TEXT("MainComputeShader"),SF_Compute);
IMPLEMENT_SHADER_TYPE(,FComputeShaderTransposeDeclaration,TEXT("/rdComputeShader/rdMeshlets.usf"),TEXT("TransposeMatrix"),SF_Compute);

IMPLEMENT_MODULE(FComputeShaderModule,ComputeShader)

//----------------------------------------------------------------------------------------------------------------

*/