#include "transformgroup.h"

void TransformGroup::deletePiece(int piece){
    auto iter = std::find(pieces.begin(), pieces.end(), piece);
    if(iter != pieces.end()){
        pieces.erase(iter);
    }
}
