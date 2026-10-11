// INCLUDE FILES
#include <e32std.h>
#include <akntitle.h> 
#include <aknappui.h>
#include <eikmenup.h>
#include <eikon.hrh>

#include "GsmPos.hrh"
#include <GsmPos.rsg>
#include "GsmPosListBoxMarkableDialog.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CGsmPosListBoxMarkableDialog::CGsmPosListBoxMarkableDialog
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CGsmPosListBoxMarkableDialog::CGsmPosListBoxMarkableDialog(
    TInt& aSelectedItem,
    CArrayFixFlat<TInt>* aMarkedItems,
    CDesCArray* aListItems)
    : 
    CAknMarkableListDialog(aSelectedItem, aMarkedItems, aListItems, -1, -1, NULL)
	{
	}

// Destructor
CGsmPosListBoxMarkableDialog::~CGsmPosListBoxMarkableDialog()
	{
    // Restore title pane. Ownership of iOriginalTitle transferred.
    if (iOriginalTitle && iTitlePane)
        {
        iTitlePane->SetText(iOriginalTitle);
        }
	}

// -----------------------------------------------------------------------------
// CGsmPosListBoxMarkableDialog::RunDialogL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
TInt CGsmPosListBoxMarkableDialog::RunDialogL(CDesCArray* aListItems, CArrayFixFlat<TInt>* aMarkedItems)
    {
    // Create the dialog
    TInt selectedItem = 0;

    CGsmPosListBoxMarkableDialog* dialog = 
        new (ELeave) CGsmPosListBoxMarkableDialog(selectedItem, aMarkedItems, aListItems);
    
	dialog->PrepareLC(R_LISTBOX_MARKABLE_DIALOG);

	// pre-mark currently selected categories 
    CEikListBox* listBox = (CEikListBox*) dialog->Control(ESelectionListControl);
    for (TInt i = 0; i < aMarkedItems->Count(); i++)
    {
        listBox->View()->SelectItemL((*aMarkedItems)[i]);
        /* TODO EIKON LISTBOX 5 panic
        if (i == 0)
        	listBox->View()->SetCurrentItemIndex((*aMarkedItems)[0]);
        */
    }

    return dialog->RunLD();
    }

// -----------------------------------------------------------------------------
// CGsmPosListBoxMarkableDialog::PostLayoutDynInitL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CGsmPosListBoxMarkableDialog::PostLayoutDynInitL()
    {
    CAknMarkableListDialog::PostLayoutDynInitL();
    
    // Save original title
    CEikStatusPane* statusPane = iAvkonAppUi->StatusPane();
    iTitlePane = (CAknTitlePane*) statusPane->ControlL(
        TUid::Uid(EEikStatusPaneUidTitle));
    iOriginalTitle = iTitlePane->Text()->AllocL();

    // Update title
	/*
    HBufC* title = iCoeEnv->AllocReadResourceAsDes16L(
        R_LMREFAPP_EDIT_CAT_TITLE);
    iTitlePane->SetText(title);
	*/
    }

// -----------------------------------------------------------------------------
// CGsmPosListBoxMarkableDialog::OfferKeyEventL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TKeyResponse CGsmPosListBoxMarkableDialog::OfferKeyEventL(
    const TKeyEvent& aKeyEvent, 
    TEventCode aType)
    {
    if (aKeyEvent.iCode == EKeyOK)
        {
		// An item should be marked/unmarked.
		CEikListBox* listBox = (CEikListBox*) Control(ESelectionListControl);
		listBox->View()->ToggleItemL(listBox->View()->CurrentItemIndex());
		listBox->DrawDeferred();

        return EKeyWasConsumed;
        }

    return CAknMarkableListDialog::OfferKeyEventL(aKeyEvent, aType);
    }

TBool CGsmPosListBoxMarkableDialog::OkToExitL(TInt aButtonId)
	{
	//sprawdz czy brak zaznaczenia!
	if (aButtonId == EAknSoftkeyOk)
		{
		CEikListBox* listBox = (CEikListBox*) Control(ESelectionListControl);
		if (!listBox->SelectionIndexes()->Count())
			return EFalse;
		}
	
	return CAknMarkableListDialog::OkToExitL(aButtonId);
	}


