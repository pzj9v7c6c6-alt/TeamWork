
#include <iostream>
#include <string>
#include <windows.h>
#undef LoadImage
#include <mmsystem.h>
#include <shellapi.h>
#include "../Application/ApplicationLayer/ImageAppController.h"

using namespace std;

void showMenu() {
    cout << "\n";
    cout << "========================================\n";
    cout << "          IMAGE PROCESSOR CLI           \n";
    cout << "========================================\n";
    cout << "  1. Black & White\n";
    cout << "  2. Blur\n";
    cout << "  3. Brightness\n";
    cout << "  4. Contrast\n";
    cout << "  5. Crop\n";
    cout << "  6. Detect Edges\n";
    cout << "  7. Flip\n";
    cout << "  8. Frame Filter\n";
    cout << "  9. Gray\n";
    cout << " 10. Hue\n";
    cout << " 11. Infrared Filter\n";
    cout << " 12. Invert\n";
    cout << " 13. Merge\n";
    cout << " 14. Resize\n";
    cout << " 15. Rotate\n";
    cout << " 16. Saturation\n";
    cout << " 17. TV Scanlines\n";
    cout << " 18. Wano Purple\n";
    cout << " 19. Wanw\n";
    cout << "  0. Exit\n";
    cout << "========================================\n";
}

bool askYesNo(const string& message) {

    while (true) {

        char answer;

        cout << message << " (y/n): ";
        cin >> answer;

        if (answer == 'y' || answer == 'Y') {
            return true;
        }

        if (answer == 'n' || answer == 'N') {
            return false;
        }

        cout << "[Error] Please enter Y or N.\n";
    }
}

int main() {

    try {

        ImageAppController controller;
        Image image;



        while (true) {



            while (true) {
                system("cls");

                string loadPath;

                cout << "\n========================================\n";
                cout << "Enter image path to load: ";
                cin >> loadPath;

                try {

                    if (controller.LoadImage(loadPath, image)) {

                        cout << "\n[Success] Image loaded successfully!\n";
                        break;
                    }

                    cout << "\n[Error] Could not load the image.\n";
                    cout << "Please check the file name/path and try again.\n";
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cin.get();
                }
                catch (const exception& ex) {

                    cout << "\n[Error] Unable to load the image.\n";
                    cout << "Reason: " << ex.what() << "\n";
                    cout << "Please try again.\n";
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cin.get();
                }
                catch (...) {

                    cout << "\n[Error] Unexpected error while loading the image.\n";
                    cout << "Please try again.\n";
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cin.get();
                }
            }



            while (true) {

                system("cls");
                showMenu();

                int choice;

                cout << "Choose a filter option: ";

                if (!(cin >> choice)) {

                    cout << "\n[Error] Invalid input.\n";
                    cout << "Please enter a number from 0 to 19.\n";

                    cin.clear();
                    cin.ignore(10000, '\n');

                    continue;
                }


                if (choice == 0) {
                    system("cls");
                    cout << "\nExiting Image Processor...\n";
                    return 0;
                }



                if (choice < 1 || choice > 19) {

                    cout << "\n[Error] Invalid filter choice.\n";
                    cout << "Please choose a number from 1 to 19.\n";
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cin.get();

                    continue;
                }

                FilterType selectedFilter =
                    static_cast<FilterType>(choice);



                try {

                    if (!controller.applyFilter(selectedFilter, image)) {

                        cout << "\n[Error] The filter could not be applied.\n";
                        cout << "Returning to the menu...\n";
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        cin.get();

                        continue;
                    }

                    cout << "\n[Success] Filter applied successfully!\n";
                }
                catch (const exception& ex) {

                    cout << "\n[Error] Failed to apply the filter.\n";
                    cout << "Reason: " << ex.what() << "\n";
                    cout << "Returning to the menu...\n";
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cin.get();

                    continue;
                }
                catch (...) {

                    cout << "\n[Error] Unexpected error while applying the filter.\n";
                    cout << "Returning to the menu...\n";
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cin.get();


                    continue;
                }



                while (true) {
                    system("cls");
                    string savePath;

                    cout << "\nEnter save path (e.g., output.jpg): ";
                    cin >> savePath;

                    try {

                        if (controller.SaveImage(image, savePath)) {

                            cout << "\n[Success] Image saved successfully!\n";
                            cout << "Location: " << savePath << "\n";

                            PlaySound(
                                TEXT("SystemExclamation"),
                                NULL,
                                SND_ALIAS | SND_ASYNC
                            );

                            wstring widestr(
                                savePath.begin(),
                                savePath.end()
                            );

                            ShellExecuteW(
                                NULL,
                                L"open",
                                widestr.c_str(),
                                NULL,
                                NULL,
                                SW_SHOWNORMAL
                            );

                            break;
                        }

                        cout << "\n[Error] Failed to save the image.\n";
                        cout << "Please check the file name/path and try again.\n";
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        cin.get();
                    }
                    catch (const exception& ex) {

                        cout << "\n[Error] Unable to save the image.\n";
                        cout << "Reason: " << ex.what() << "\n";
                        cout << "Please enter another save path.\n";
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        cin.get();
                    }
                    catch (...) {

                        cout << "\n[Error] Unexpected error while saving.\n";
                        cout << "Please enter another save path.\n";
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        cin.get();
                    }
                }

                if (askYesNo("\nDo you want to apply another filter?")) {
                    continue;
                }



                if (askYesNo("\nDo you want to load another image?")) {
                    break;
                }

                system("cls");
                return 0;
            }


            continue;
        }

    }
    catch (const exception& ex) {

        cout << "\n========================================\n";
        cout << "Unexpected application error.\n";
        cout << "Reason: " << ex.what() << "\n";
        cout << "The application will remain open.\n";
        cout << "========================================\n";

        system("pause");
    }
    catch (...) {

        cout << "\n========================================\n";
        cout << "Unexpected application error.\n";
        cout << "The application will remain open.\n";
        cout << "========================================\n";

        system("pause");
    }

    return 0;
}
