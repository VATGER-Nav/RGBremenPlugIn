#include "SquawkHandler.h"

bool SquawkHandler::IsSquawkVFR(int squawk)
{
	switch (squawk)
	{
	case 0:
	case 20:
	case 23:
	case 24:
	case 25:
	case 27:
	case 30:
	case 31:
	case 33:
	case 34:
	case 35:
	case 36:
	case 37:
	case 76:
	case 7000:
	case 7740:
	case 7741:
	case 7742:
	case 7743:
	case 7744:
	case 7745:
		return true;
	default:
		break;
	}
	return false;
}
