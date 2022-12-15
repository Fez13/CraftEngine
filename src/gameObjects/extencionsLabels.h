#pragma once

#include <iostream>


namespace craft{
    using extencion = std::uint16_t;

    namespace Extencions{
        enum : extencion{
            CR_NULL_EXTENCION = 0,
            CR_TRANSFORM_EXTENCION = 1,
            CR_UPDATE_EXTENCION = 2,
            CR_CAMERACONTROLLER_EXTENCION = 3,
            CR_CAMERA_EXTENCION = 4,
            
        };
    }
}
