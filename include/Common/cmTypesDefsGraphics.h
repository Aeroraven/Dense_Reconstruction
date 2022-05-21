#pragma once
#include "./cmTypeDefs.h"

//GLM
#define glm_printmat4(x) for(i32 _i=0;_i<4;_i++){for(i32 _j=0;_j<4;_j++)std::cout<<x[_i][_j]<<" ";std::cout<<std::endl;}

namespace Common{
    namespace Camera{
        //This structure does not applies to the computer vision tasks
        //It's intended for graphical draws.
        //For vision tasks, use intrinsic & extrinsic instead.
        struct CoordinateSystems{
            glm::mat4 modelMatrix; //This transforms objects' local CS to world CS
            glm::mat4 viewMatrix; //This transforms world CS to camera CS
            glm::mat4 projectionMatrix; //This performs projection 
        };
    }

    namespace Mesh{
        struct ShaderCompatibleMeshData{
            f32* vertexData = nullptr; 
            u32* faceData = nullptr;
        };
    }
}