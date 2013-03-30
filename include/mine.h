// ��a�p�C�� Ver 1.0
// mine.h �i���O�쫬�ŧi�j

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define TRUE	1
#define	FALSE	0

// �a�p�Ϫ��j�p
#define X_MAX	30
#define	Y_MAX	15

// �a�p���w�q
#define	DEF_MINE		-1	// �a�p�N��

// MineStruct ���� Status �w�q
#define	NO_TOUCH		0	// ����L
#define TOUCH_OK		1	// �w������L
#define TOUCH_ERR		2	// ���a�p
#define	MARK_MINE		3	// �Q�Хܬ��a�p

struct MineStruct
{
	char	MineOrNo;		// �a�p���,�O���Ӧ�m�O�a�p�٬O�Ʀr
	char	Status;			// �a�p���A,�O���C�����Ӧ�m�����A
};

// ��a�p�C�������O�w�q
class MineGame
{
	private:
		MineStruct MineMap[X_MAX][Y_MAX];	// �a�p��
		int TotalMine;				// �����a�p��		
		int MarkedMine;				// �w�Хܪ��a�p��
		int BuryOneMine(int, int);		// �I�]�a�p
		int OpenBlankBlock(int, int);		// �}�Ҫťհ϶�
	public:
		MineGame();
		MineGame(int);
		int InitMineMap(int);		// �}�Ҥ@�Ӧa�p��
		int MarkMine(int, int);		// �Хܦa�p
		int UnMarkMine(int, int);	// �����a�p�Х�
		int OpenMine(int, int);		// ��a�p
		int ReadMineStatus(int, int);	// Ū���a�p���A
		int ReadMineData(int, int);	// Ū���a�p���
		int CountMarkedMine();		// Ū���Q�Хܪ��a�p��
		int CheckMineMap();		// �ˬd�a�p�ϥܬO�_�ŦX
		int OpenNearMine(int &, int &);	// �ֳt��P�򪺦a�p
		~MineGame();
};

