#include <gtk/gtk.h>
#include <desktop/XMLOptions.h>
#include <gtk/GtkDialogManager.h>

#include "GtkFBReader.h"
#include "GtkWord.h"

int main(int argc, char **argv) {
	gtk_init(&argc, &argv);

	XMLOptions::createInstance("FBReader");
	GtkDialogManager::createInstance();
	GtkWordBuilder::createInstance();

	new GtkFBReader();
	gtk_main();

	GtkWordBuilder::deleteInstance();
	GtkDialogManager::deleteInstance();
	XMLOptions::deleteInstance();

	return 0;
}
