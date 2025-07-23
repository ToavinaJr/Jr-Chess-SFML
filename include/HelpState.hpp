#pragma once

#include "GameState.hpp"
#include "FontManager.hpp"
#include "Button.hpp"
#include <vector>
#include <string>
#include <memory>

namespace Jr {

/**
 * @class HelpState
 * @brief État du jeu affichant l'écran d'aide ou de tutoriel.
 * 
 * Cette classe dérive de GameState et gère l'affichage d'un écran contenant un titre,
 * un contenu textuel explicatif, ainsi qu'un bouton pour revenir à l'état précédent.
 */
class HelpState : public GameState {
private:
    /// Texte du titre affiché en haut de l'écran d'aide
    sf::Text titleText;

    /// Texte du contenu affiché dans la zone d'aide
    sf::Text contentText;

    /// Référence au gestionnaire de polices pour gérer les polices utilisées
    FontManager& fontManager;

    /// Bouton permettant de revenir à l'état précédent ou menu
    std::unique_ptr<Button> backButton;

    /**
     * @brief Centre un objet texte horizontalement sur la fenêtre avec un décalage vertical.
     * 
     * @param text Texte SFML à centrer.
     * @param yOffset Décalage vertical depuis le haut de la fenêtre.
     */
    void centerText(sf::Text& text, float yOffset);

public:
    /**
     * @brief Constructeur de l'état HelpState.
     * 
     * Initialise les textes, le bouton, et référence les managers nécessaires.
     * 
     * @param manager Référence au gestionnaire d'états pour changer d'état.
     * @param win Référence à la fenêtre SFML de rendu.
     * @param fm Référence au gestionnaire de polices.
     */
    HelpState(StateManager& manager, sf::RenderWindow& win, FontManager& fm);

    /// Destructeur par défaut virtuel.
    ~HelpState() override = default;

    /**
     * @brief Gère les entrées utilisateur (clavier, souris, etc.) pour cet état.
     * @param event Événement SFML à traiter.
     */
    void handleInput(const sf::Event& event) override;

    /**
     * @brief Met à jour la logique de l'état (animations, transitions, etc.).
     * @param deltaTime Temps écoulé depuis la dernière mise à jour (en secondes).
     */
    void update(float deltaTime) override;

    /**
     * @brief Dessine tous les éléments graphiques liés à cet état sur la fenêtre.
     */
    void draw() override;

    /**
     * @brief Appelé quand cet état devient actif.
     * Permet d'initialiser ou réinitialiser certains éléments avant l'affichage.
     */
    void onEnter() override;
};

} // namespace Jr
