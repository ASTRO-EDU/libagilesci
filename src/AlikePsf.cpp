

/** zzz Debug
#include <iostream>
using std::cout;
using std::cerr;
using std::endl;
*/

#include "wcstrig.h"

#include "AlikePsf.h"


////////////////////////////////////////////////////////////
///
/// Class AlikeCircle
///


/**

Static Functions for AlikeCircle::Reset()

FirstXxxBin() look for the first bin coming from outside the circle
LastXxxBin() and Top/BottmBin() look for the last bin coming from inside the circle
Top() finds the topmost row inside the circle, and its left and right boundary
Bottom() finds the bottommost row inside the circle
MoveLeftBoundary() and MoveRightBoundary() find the boundaries for an adjacent row
*/


static int FirstLeftBin(double lCenter, double bCenter, double rAnal, const AgileMap& agileMap, int row, int col)
{
int lastCol = agileMap.Cols()-1;
int left = col;
while (left<lastCol)
	if (agileMap.SrcDist(row, left+1, lCenter, bCenter)>=rAnal)
		++left;
	else
		return left+1;
return -1;
}


static int LastLeftBin(double lCenter, double bCenter, double rAnal, const AgileMap& agileMap, int row, int col)
{
int left = col;
while (left>0)
	if (agileMap.SrcDist(row, left-1, lCenter, bCenter)<rAnal)
		--left;
	else
		break;
return left;
}


static bool MoveLeftBoundary(double lCenter, double bCenter, double rAnal, const AgileMap& agileMap, int row, int& left)
{
if (agileMap.SrcDist(row, left, lCenter, bCenter)<rAnal)
	left = LastLeftBin(lCenter, bCenter, rAnal, agileMap, row, left);
else
	left = FirstLeftBin(lCenter, bCenter, rAnal, agileMap, row, left);
return left!=-1;
}


static int FirstRightBin(double lCenter, double bCenter, double rAnal, const AgileMap& agileMap, int row, int col)
{
int right = col;
while (right>0)
	if (agileMap.SrcDist(row, right-1, lCenter, bCenter)>=rAnal)
		--right;
	else
		return right-1;
return -1;
}


static int LastRightBin(double lCenter, double bCenter, double rAnal, const AgileMap& agileMap, int row, int col)
{
int lastCol = agileMap.Cols()-1;
int right = col;
while (right<lastCol)
	if (agileMap.SrcDist(row, right+1, lCenter, bCenter)<rAnal)
		++right;
	else
		break;
return right;
}

static bool MoveRightBoundary(double lCenter, double bCenter, double rAnal, const AgileMap& agileMap, int row, int& right)
{
if (agileMap.SrcDist(row, right, lCenter, bCenter)<rAnal)
	right = LastRightBin(lCenter, bCenter, rAnal, agileMap, row, right);
else
	right = FirstRightBin(lCenter, bCenter, rAnal, agileMap, row, right);
return right!=-1;
}



static int TopBin(double lCenter, double bCenter, double rAnal, const AgileMap& agileMap, int row, int col)
{
int top = row;
while (top>0)
	if (agileMap.SrcDist(top-1, col, lCenter, bCenter)<rAnal)
		--top;
	else
		break;
return top;
}


static int BottomBin(double lCenter, double bCenter, double rAnal, const AgileMap& agileMap, int row, int col)
{
int lastRow = agileMap.Rows()-1;
int bottom = row;
while (bottom<lastRow)
	if (agileMap.SrcDist(bottom+1, col, lCenter, bCenter)<rAnal)
		++bottom;
	else
		break;
return bottom;
}


/// Look for a shorter range of bins within left and right
/// Return true is at least on bin was found
static bool CheckBlock(double lCenter, double bCenter, double rAnal, const AgileMap& agileMap, int row, int& left, int& right)
{
int left2 = left;
int right2 = right;
bool binsFound = false;
for (int i=left; i<=right; ++i)
	if (agileMap.SrcDist(row, i, lCenter, bCenter)<rAnal) {
		if (!binsFound) {
			binsFound = true;
			left2 = i;
			}
		right2 = i;
		}
	else if (binsFound)
		break;
if (binsFound) {
	left = left2;
	right = right2;
	}
return binsFound;
}

static int Top(double lCenter, double bCenter, int row, int col, double rAnal, const AgileMap& agileMap, int& left, int& right)
{
int top = TopBin(lCenter, bCenter, rAnal, agileMap, row, col);
left = LastLeftBin(lCenter, bCenter, rAnal, agileMap, top, col);
right = LastRightBin(lCenter, bCenter, rAnal, agileMap, top, col);
while (top>0 && CheckBlock(lCenter, bCenter, rAnal, agileMap, top-1, left, right))
	--top;
return top;
}


static int Bottom(double lCenter, double bCenter, int row, int col, double rAnal, const AgileMap& agileMap)
{
int bottom = BottomBin(lCenter, bCenter, rAnal, agileMap, row, col);
int left = LastLeftBin(lCenter, bCenter, rAnal, agileMap, bottom, col);
int right = LastRightBin(lCenter, bCenter, rAnal, agileMap, bottom, col);
int lastRow = agileMap.Rows()-1;

while (bottom<lastRow && CheckBlock(lCenter, bCenter, rAnal, agileMap, bottom+1, left, right))
	++bottom;

return bottom;
}


void AlikeCircle::Reset(double lCenter, double bCenter, double rAnal, const AgileMap& agileMap)
{
/*
m_lCenter = lCenter;
m_bCenter = bCenter;
m_rAnal = rAnal;
*/

m_map.ResizeTo(0);

int rowCenter;
int colCenter;
bool inside = agileMap.GetRowCol(lCenter, bCenter, &rowCenter, &colCenter);
if (!inside) {
	if (rowCenter<0)
		rowCenter = 0;
	else {
		int lastRow = agileMap.Rows()-1;
		if (rowCenter>lastRow)
			rowCenter = lastRow;
		}
	if (colCenter<0)
		colCenter = 0;
	else {
		int lastCol = agileMap.Cols()-1;
		if (colCenter>lastCol)
			colCenter = lastCol;
		}
	inside = agileMap.SrcDist(rowCenter, colCenter, lCenter, bCenter)<rAnal;
	}
if (inside) {
	int leftUp;
	int rightUp;
	int top = Top(lCenter, bCenter, rowCenter, colCenter, rAnal, agileMap, leftUp, rightUp);
	int bottom = Bottom(lCenter, bCenter, rowCenter, colCenter, rAnal, agileMap);
	VecI leftArr(bottom-top+1);
	VecI rightArr(bottom-top+1);
	int left = leftArr[0] = leftUp;
	int right = rightArr[0] = rightUp;
	/// cerr << "DEBUG: top=" << top << " left=" << left << " right=" << right << " bottom=" << bottom << endl;
	int count = right-left+1;
	for (int r=top+1; r<=bottom; ++r) {
		if (MoveLeftBoundary(lCenter, bCenter, rAnal, agileMap, r, left)
					&& MoveRightBoundary(lCenter, bCenter, rAnal, agileMap, r, right)) {
			leftArr[r-top] = left;
			rightArr[r-top] = right;
			}
		count += right-left+1;
		}
	m_map.ResizeTo(count);
	int index = 0;
	for (int i=0; i<=bottom-top; ++i)
		for (int c=leftArr[i]; c<=rightArr[i]; ++c)
			m_map[index++] = agileMap.Bin(top+i, c);
	}
}





////////////////////////////////////////////////////
///
/// Class AlikeNorm
///


void AlikeNorm::UpdateNorm(double eMin, double eMax, double index)
{
index = 1.0-index;
m_normFactor = (pow(eMin, index)-pow(eMax, index))/(pow(m_eInf, index)-pow(m_eSup, index));
}




////////////////////////////////////////////////////
///
/// Class AlikePsfTables
///



bool AlikePsfTables::Read(const char* psfFileName, const char* raeffFileName, const char* edpFileName)
{
m_psfFileName = psfFileName;
m_raeffFileName = raeffFileName;
m_edpFileName = edpFileName;

bool ok = !AeffGrid::Read(raeffFileName);
ok = ok && !PsfGrid::Read(psfFileName);
ok = ok && !EdpGrid::Read(edpFileName);
ok = ok && PsfeCount() && RhoCount()>1 && Rho(0)!=Rho(1);
m_deltaRho = 1;
if (ok) {
	m_deltaRho = Rho(1)-Rho(0);
	m_sinRhoArr.ResizeTo(PsfGrid::m_psfrho);
	for (int i=0; i<m_sinRhoArr.Size(); i++)
		m_sinRhoArr[i] = sind(PsfGrid::m_psfrho[i]);
	}
return ok;
}









////////////////////////////////////////////////////////////////////////
///
/// Class AlikePsfSource
///
///


void AlikePsfSource::Set(
	const AlikePsfTables* psfTab,
	const AgileMap& map,
	double eInf,
	double eSup,
	double theta,
	double srcL,
	double srcB,
	double index)
{
m_psfTab = psfTab;
AgileMap::operator=(map);
SetEnergyRange(eInf, eSup);
m_theta = theta;

int rhoCount = m_psfTab->RhoCount();
int psfeCount = m_psfTab->PsfeCount();

const VecF& psfEnergies = m_psfTab->PsfEnerges();
int lowetrue = psfEnergies.GeomIndex(GetEmin());
int highetrue = psfEnergies.GeomIndex(GetEmax());

/// Preparing the working arrays 
m_edpArr.ResizeTo(psfeCount);
m_specwt.ResizeTo(psfeCount);
m_psfArr.ResizeTo(rhoCount);
m_edpArr = 0.0;
m_specwt = 0.0;
m_psfArr = 0.0;

/// Calcolo della dispersione energetica totale per ogni canale di energia
for (int etrue=0; etrue<psfeCount; etrue++)
	for (int eobs=lowetrue;  eobs<=highetrue; eobs++)
		m_edpArr[etrue] += m_psfTab->EdpVal(psfEnergies[etrue], psfEnergies[eobs], m_theta, 0.0);

/** zzz Debug
cout << "rhoCount: " << rhoCount << endl;
cout << "psfeCount: " << psfeCount << endl;

cout << "m_theta: " << m_theta << endl;

cout << "m_index: " << m_index << endl;
cout << "m_srcL: " << m_srcL << endl;
cout << "m_srcB: " << m_srcB << endl;

cout << "index: " << index << endl;
cout << "srcL: " << srcL << endl;
cout << "srcB: " << srcB << endl;
*/

SetSrcData(srcL, srcB, index, true);
}




AlikePsfSource::Changes AlikePsfSource::SetSrcData(double srcL, double srcB, double index, bool force)
{
if (index<0)
	index = -index;
if (!force && index==m_index && srcL==m_srcL && srcB==m_srcB)
	return NoChanges;

Changes changes = NoChanges;
if (index!=m_index || force) {
	changes = IndexChanged;

	m_index = index;
	UpdateNorm(GetEmin(), GetEmax(), m_index);
	
	int rhoCount = m_psfTab->RhoCount();
	int psfeCount = m_psfTab->PsfeCount();

	/// Calcolo del peso di ogni energia in base all'indice spettrale
	const VecF& psfEnergies = m_psfTab->PsfEnerges();
	for (int i=0; i<psfeCount-1; ++i)
		m_specwt[i] = pow(psfEnergies[i], 1.0-m_index) - pow(psfEnergies[i+1], 1.0-m_index);
	m_specwt[psfeCount-1] = pow(psfEnergies[psfeCount-1], 1.0-m_index);

	/// Calcolo della psf da normalizzare
	m_psfArr = 0.0;
	for (int etrue=0; etrue<psfeCount; ++etrue) {
		double energyDep = m_psfTab->AeffVal(psfEnergies[etrue], m_theta, 0.0) * m_edpArr[etrue] * m_specwt[etrue];
		for (int i=0; i<rhoCount; ++i)
			m_psfArr[i] += energyDep * m_psfTab->PsfVal(m_psfTab->Rho(i), 0.0, m_theta, 0.0, psfEnergies[etrue]);
		}

	/// Normalizzazione della psf secondo l'area di ogni corona circolare
	double dsum = 0.0;
	const VecF& sinRhos = m_psfTab->SinRhos();
	for (int i=0; i<rhoCount; i++)
		dsum += m_psfArr[i] * sinRhos[i];
	double d = dsum * 2 * M_PI * m_psfTab->DeltaRho();
	for (int i=0; i<rhoCount; i++)
		m_psfArr[i] /= d;
	}

if (srcL!=m_srcL || srcB!=m_srcB || force) {
	m_srcL = srcL;
	m_srcB = srcB;
	changes = Changes(changes | PositionChanged);
	}

/// Assumptions:
/// 	m_rhoArr[i+1]-m_rhoArr[i] = deltaRho = constant for each i
/// 	m_rhoArr[i] individua energie nell'intervallo [ rho[i]-deltaRho .. rho[i]+deltaRho [
/// 		and therefore:
/// 	rho[0] = deltaRho/2
if (changes!=NoChanges) {

	const VecF& rhoArr = m_psfTab->Rhos();

	double deltaRho = m_psfTab->DeltaRho();
	int psfCount = m_psfTab->RhoCount();
	double maxRho = rhoArr[psfCount-1]+deltaRho/2;

	Zero();
	AlikeCircle circle(m_srcL, m_srcB, maxRho, *this);

	int count = circle.GetNoElements();
/*
	cerr << "OK, count=" << count << ", deltaRho=" << deltaRho << endl;
	cerr << "Rows=" << Rows() << ", Cols=" << Cols() << endl;
*/
	for (int c=0; c<count; ++c) {
		int bin = circle[c];
/*
		if (bin>28552) {
			cerr << "bin=" << bin;
			cerr << ", i(bin)=" << i(bin);
			cerr << ", j(bin)=" << j(bin);
			cerr << ", l(bin)=" << l(bin);
			cerr << ", b(bin)=" << b(bin);
			}
*/
		double srcdist = SphDistDeg(m_srcL, m_srcB, l(bin), b(bin));
		int psfind = srcdist/deltaRho;
		double resid = (srcdist - rhoArr[psfind])/deltaRho;
		double correction;
		if (resid<0)
			if (psfind==0)
				correction = m_psfArr[1]-m_psfArr[0];
			else
				correction = m_psfArr[psfind]-m_psfArr[psfind-1];
		else
			if (psfind==psfCount-1)
				correction = -m_psfArr[psfind];	/// Next one would be zero
			else
				correction = m_psfArr[psfind+1]-m_psfArr[psfind];
		double psflookup = m_psfArr[psfind] + resid * correction;
		int ii = i(bin);
		int jj = j(bin);
		operator()(ii,jj) = Area(ii,jj) * psflookup * RAD2DEG;
		}
	}
return changes;
}

