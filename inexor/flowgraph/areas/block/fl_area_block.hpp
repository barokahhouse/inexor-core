/// @file fl_area_block.h
/// @author Johannes Schneider
/// @brief 

#ifndef INEXOR_VSCRIPT_AREA_BLOCK_HEADER
#define INEXOR_VSCRIPT_AREA_BLOCK_HEADER

#include "inexor/flowgraph/nodebase/fl_base.hpp"


namespace inexor {
namespace vscript {

    class CCubeAreaNode : public CScriptNode
    {
        protected:
            
            float box_width;
            float box_height;
            float box_depth;

        public:

            CCubeAreaNode(vec,float,float,float,const char*,const char*);
            ~CCubeAreaNode();
            
            bool collide(vec p);
            void render_additional();

            bool OnLinkAsChildNodeAttempt(CScriptNode* parent);
            bool OnLinkAsParentNodeAttempt(CScriptNode* child);
    };

};
};

#endif

