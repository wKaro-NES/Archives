
#ifndef __LANDMARKS_CATEGORIES_DIALOG_H__
#define __LANDMARKS_CATEGORIES_DIALOG_H__

// INCLUDES
#include <e32base.h>
#include <aknselectionlist.h>

// FORWARD DECLARATIONS
class CAknTitlePane;

// CLASS DECLARATION

class CGsmPosListBoxMarkableDialog : public CAknMarkableListDialog
	{
    public: // Constructors and destructor

        /**
        * Creates, executes and destroys the dialog after dismissed.
        * @param 
        * @return EFalse if dialog was cancelled
        */
        static TInt RunDialogL(CDesCArray* aListItems, CArrayFixFlat<TInt>* aMarkedItems);

        /**
        * Destructor.
        */
	    ~CGsmPosListBoxMarkableDialog();

    protected: // From CEikDialog

        /**
        * Called after the dialog is layed out.
        */
        void PostLayoutDynInitL();

        /**
        * OfferKeyEventL handles key events.
        *
        * @param aKeyEvent the key event
        * @param aType the type of key event
        * @return EKeyWasConsumed if keyevent was handled, EKeyWasNotConsumed 
        * otherwise 
        */
		TKeyResponse OfferKeyEventL(
            const TKeyEvent& aKeyEvent,
            TEventCode aType);

	   /** OkToExitL()
		* From CAknDialog.
		* This is not called if the Cancel button is activated unless the EEikDialogFlagNotifyEsc flag has been set.
		* @param aButtonId The ID of the button that was activated.
		*/
		TBool OkToExitL(TInt aButtonId);
		
    private:

        /**
        * C++ constructor.
        *
        * @param aSelectedItem the selected item.
        * @param aMarkedItems an array of selected categories.
        * @param aListItems array of items to display.
        */
	    CGsmPosListBoxMarkableDialog(
            TInt& aSelectedItem,
            CArrayFixFlat<TInt>* aMarkedItems,
            CDesCArray* aListItems);

    private: // Data

        //! The title pane that is dynamically updated
        CAknTitlePane* iTitlePane;

        //! The original title that the title pane displayed before modified
        HBufC* iOriginalTitle;
	};

#endif 

