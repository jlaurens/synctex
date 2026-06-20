#include <iostream>
#include <filesystem>
#include <string>

namespace fs = std::filesystem;

int main(int argc, char* argv[]) {
    std::string chemin;
    if (argc < 2) {
        std::cerr << "** Usage: " << argv[0] << " <chemin>" << std::endl;
        chemin = "../..";
    } else {
        chemin = argv[1];
    }

    if (!fs::exists(chemin)) {
        std::cerr << "Erreur: Le chemin '" << chemin << "' n'existe pas." << std::endl;
        return 1;
    }

    if (fs::is_directory(chemin)) {
        std::cout << "Dossier :" << std::endl;
        std::cout << fs::canonical(chemin) << std::endl;
        std::cout << "Contenu :" << std::endl;
        for (const auto& entry : fs::directory_iterator(chemin)) {
            std::cout << "- " << entry.path().filename() << std::endl;
        }
    } else {
        std::cout << "Fichier :" << std::endl;
        std::cout << fs::canonical(chemin) << std::endl;
        std::cout << "Taille: "
                  << fs::file_size(chemin) << " octets." << std::endl;
    }

    return 0;
}
