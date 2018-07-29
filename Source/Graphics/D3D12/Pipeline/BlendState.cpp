#include "BlendState.h"

using namespace Engine;

D3D12_BLEND BlendState::BlendModeD3D[] =
{
	D3D12_BLEND_ZERO,
	D3D12_BLEND_ONE,
	D3D12_BLEND_SRC_COLOR,
	D3D12_BLEND_INV_SRC_COLOR,
	D3D12_BLEND_SRC_ALPHA,
	D3D12_BLEND_INV_SRC_ALPHA,
	D3D12_BLEND_DEST_ALPHA,
	D3D12_BLEND_INV_DEST_ALPHA,
	D3D12_BLEND_DEST_COLOR,
	D3D12_BLEND_INV_DEST_COLOR,
	D3D12_BLEND_SRC_ALPHA_SAT,
	D3D12_BLEND_BLEND_FACTOR,
	D3D12_BLEND_INV_BLEND_FACTOR,
	D3D12_BLEND_SRC1_COLOR,
	D3D12_BLEND_INV_SRC1_COLOR,
	D3D12_BLEND_SRC1_ALPHA,
	D3D12_BLEND_INV_SRC1_ALPHA
};

D3D12_BLEND_OP BlendState::BlendOpD3D[] =
{
	D3D12_BLEND_OP_ADD,
	D3D12_BLEND_OP_SUBTRACT,
	D3D12_BLEND_OP_REV_SUBTRACT,
	D3D12_BLEND_OP_MIN,
	D3D12_BLEND_OP_MAX
};

D3D12_LOGIC_OP BlendState::LogicOpD3D[] =
{
	D3D12_LOGIC_OP_CLEAR,
	D3D12_LOGIC_OP_SET,
	D3D12_LOGIC_OP_COPY,
	D3D12_LOGIC_OP_COPY_INVERTED,
	D3D12_LOGIC_OP_NOOP,
	D3D12_LOGIC_OP_INVERT,
	D3D12_LOGIC_OP_AND,
	D3D12_LOGIC_OP_NAND,
	D3D12_LOGIC_OP_OR,
	D3D12_LOGIC_OP_NOR,
	D3D12_LOGIC_OP_XOR,
	D3D12_LOGIC_OP_EQUIV,
	D3D12_LOGIC_OP_AND_REVERSE,
	D3D12_LOGIC_OP_AND_INVERTED,
	D3D12_LOGIC_OP_OR_REVERSE,
	D3D12_LOGIC_OP_OR_INVERTED
};