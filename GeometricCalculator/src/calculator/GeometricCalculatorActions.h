#include <ZLApplication.h>

class GeometricCalculator;
class DiagramView;

class QuitAction : public ZLApplication::Action {

public:
	QuitAction(GeometricCalculator &calculator);
	void run();

private:
	GeometricCalculator &myCalculator;
};

class ZoomAction : public ZLApplication::Action {

public:
	ZoomAction(DiagramView &view, double coefficient);
	void run();
	bool isEnabled();

private:
	DiagramView &myView;
	const double myCoefficient;
};
