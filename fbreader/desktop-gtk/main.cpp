#include <gtk/gtk.h>

#include <desktop/XMLOptions.h>
#include <gtk/GtkDialogManager.h>
#include <gtk/GtkScreenSize.h>

#include "GtkFBReader.h"
#include "GtkWord.h"

int main(int argc, char **argv) {
	gtk_init(&argc, &argv);

	XMLOptions::createInstance("FBReader");
	GtkDialogManager::createInstance();
	GtkWordBuilder::createInstance();
	GtkScreenSize::createInstance();

	new GtkFBReader();
	gtk_main();

	GtkScreenSize::deleteInstance();
	GtkWordBuilder::deleteInstance();
	GtkDialogManager::deleteInstance();
	XMLOptions::deleteInstance();

	return 0;
}
