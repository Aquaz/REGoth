//
// Created by desktop on 08.12.16.
//

#pragma once
#include <string>
#include <list>
#include <vector>
#include <functional>
#include <map>

namespace UI
{
    class Console
    {
    public:

        // Takes list of arguments as parameter, returns command result
        typedef std::function<std::string(const std::vector<std::string>&)> CommandCallback;

        Console();

        /**
         * Updates and draws the console
         */
        void update();

        /**
         * To be called when a key got pressed.
         * @param glfwKey Key ID, glfw style
         */
        void onKeyDown(int glfwKey);

        /**
         * Adds a command to the console
         * @param Command Command to be added.
         * @param Callback Function to be executed if the given command was typed.
         *
         */
        void registerCommand(const std::string& command, CommandCallback callback);

        /**
         * Adds an autocomplete-function for an already registered command. Will get all arguments passed.
         * The last one is most likely incomplete.
         * @param command Command to add the autocomplete for
         * @param callback Callback triggered on autocomplete
         */
        void registerAutocompleteFn(const std::string& command, CommandCallback callback);


        /**
         * Executes a given command
         * @param command Command to execute
         * @return Output of the command.
         *         - "NOTFOUND", if the command was not found
         */
        std::string submitCommand(const std::string& command);

        /**
         * Adds a message to the history
         * @param msg Message to add
         */
        void historyAdd(const std::string& msg);

        /**
         * @return Whether the console is currently shown
         */
        bool isOpen(){ return m_IsOpen; }
        void setOpen(bool open){ m_IsOpen = open; }

    private:

        /**
         * Draws history over the commandline
         */
        void printHistory();

        struct
        {
            /**
             * Lines of history + current line
             */
            int height;
        }m_Config;

        /**
         * Last submitted commands
         */
        std::list<std::string> m_History;

        /**
         * Lines emitted by the commands + history
         */
        std::list<std::string> m_Output;

        /**
         * All registered commands
         */
        std::map<std::string, CommandCallback> m_Commands;

        /**
         * Callbacks for when the user wants to autocomplete an argument
         */
        std::map<std::string, CommandCallback> m_AutocompleteCallbacks;

        /**
         * Currently typed line
         */
        std::string m_TypedLine;

        /**
         * Whether the console is currently shown
         */
        bool m_IsOpen;
    };
}

