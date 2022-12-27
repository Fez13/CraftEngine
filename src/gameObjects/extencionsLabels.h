#pragma once

#include <iostream>


namespace craft{
    using extencion = std::uint16_t;

    namespace Extencions{
        enum : extencion{
            CR_EXTENCION_NULL = 0,
            CR_EXTENCION_TRANSFORM = 1,
            CR_EXTENCION_UPDATE = 2,
            CR_EXTENCION_CAMERACONTROLLER = 3,
            CR_CAMERA_EXTENCION = 4,
            CR_EXTENCION_DRAWABLE3D = 5,
        };
    }
}
