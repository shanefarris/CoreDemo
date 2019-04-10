// OpenSteerDemo PlugIn class
//
// Provides AbstractPlugIn a pure abstract base class, and PlugIn a partial
// implementation providing default methods to be sub-classed by the
// programmer defining a new "MyPlugIn".

// ----------------------------------------------------------------------------
// A pattern for a specific user-defined OpenSteerDemo PlugIn class called Foo.
// Defines class FooPlugIn, then makes a single instance (singleton) of it.
/*


class FooPlugIn : public PlugIn
{
    // required methods:
    const char* name (void) {return "Foo";}
    void open (void) {...}
    void update (const float currentTime, const float elapsedTime) {...}
    void redraw (const float currentTime, const float elapsedTime) {...}
    void close (void) {...}
    const AVGroup& allVehicles (void) {...}

    // optional methods (see comments in AbstractPlugIn for explanation):
    void reset (void) {...} // default is to reset by doing close-then-open
    float selectionOrderSortKey (void) {return 1234;}
    bool requestInitialSelection (void) {return true;}
    void handleFunctionKeys (int keyNumber) {...} // fkeys reserved for PlugIns
    void printMiniHelpForFunctionKeys (void) {...} // if fkeys are used
};

FooPlugIn gFooPlugIn;
*/

#ifndef OPENSTEER_PLUGIN_H
#define OPENSTEER_PLUGIN_H

#include <iostream>
#include "AbstractVehicle.h"

    class AbstractPlugIn
    {
    public:
        
        virtual ~AbstractPlugIn() { /* Nothing to do. */ }
        
        // generic PlugIn actions: open, update, redraw, close and reset
        virtual void open (void) = 0;
        virtual void update (const float currentTime, const float elapsedTime) = 0;
        virtual void redraw (const float currentTime, const float elapsedTime) = 0;
        virtual void close (void) = 0;
        virtual void reset (void) = 0;

        // return a pointer to this instance's character string name
        virtual const char* name (void) = 0;

        // numeric sort key used to establish user-visible PlugIn ordering
        // ("built ins" have keys greater than 0 and less than 1)
        virtual float selectionOrderSortKey (void) = 0;

        // allows a PlugIn to nominate itself as OpenSteerDemo's initially selected
        // (default) PlugIn, which is otherwise the first in "selection order"
        virtual bool requestInitialSelection (void) = 0;

        // handle function keys (which are reserved by SterTest for PlugIns)
        virtual void handleFunctionKeys (int keyNumber) = 0;

        // print "mini help" documenting function keys handled by this PlugIn
        virtual void printMiniHelpForFunctionKeys (void) = 0;

        // return an AVGroup (an STL vector of AbstractVehicle pointers) of
        // all vehicles(/agents/characters) defined by the PlugIn
		virtual const Core::AI::AVGroup& allVehicles (void) = 0;
    };


    class PlugIn : public AbstractPlugIn
    {
    public:
        // prototypes for function pointers used with PlugIns
        typedef void (* plugInCallBackFunction) (PlugIn& clientObject);
        typedef void (* voidCallBackFunction) (void);
        typedef void (* timestepCallBackFunction) (const float currentTime,
                                                   const float elapsedTime);

        // constructor
        PlugIn (void);

        // destructor
        virtual ~PlugIn();

        // default reset method is to do a close then an open
        void reset (void) {close (); open ();}

        // default sort key (after the "built ins")
        float selectionOrderSortKey (void) {return 1.0f;}

        // default is to NOT request to be initially selected
        bool requestInitialSelection (void) {return false;}

        // default function key handler: ignore all
        // (parameter names commented out to prevent compiler warning from "-W")
        void handleFunctionKeys (int /*keyNumber*/) {}

        // default "mini help": print nothing
        void printMiniHelpForFunctionKeys (void) {}

        // returns pointer to the next PlugIn in "selection order"
        PlugIn* next (void);

        // format instance to characters for printing to stream
        friend std::ostream& operator<< (std::ostream& os, PlugIn& pi)
        {
            os << "<PlugIn " << '"' << pi.name() << '"' << ">";
            return os;
        }

        // CLASS FUNCTIONS

        // search the class registry for a Plugin with the given name
        static PlugIn* findByName (const char* string);

        // apply a given function to all PlugIns in the class registry
        static void applyToAll (plugInCallBackFunction f);

        // sort PlugIn registry by "selection order"
        static void sortBySelectionOrder (void);

        // returns pointer to default PlugIn (currently, first in registry)
        static PlugIn* findDefault (void);

    private:

        // save this instance in the class's registry of instances
        void addToRegistry (void);

        // This array stores a list of all PlugIns.  It is manipulated by the
        // constructor and destructor, and used in findByName and applyToAll.
        static const int totalSizeOfRegistry;
        static int itemsInRegistry;
        static PlugIn* registry[];
    };

    
#endif // OPENSTEER_PLUGIN_H
