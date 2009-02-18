/****************************************************************************
 * Snes9x 1.51 Nintendo Wii/Gamecube Port
 *
 * Tantric February 2009
 *
 * gui_optionbrowser.cpp
 *
 * GUI class definitions
 ***************************************************************************/

#include "gui.h"
#include "filebrowser.h"

/**
 * Constructor for the GuiOptionBrowser class.
 */
GuiOptionBrowser::GuiOptionBrowser(int w, int h, OptionList * l)
{
	width = w;
	height = h;
	options = l;
	selectable = true;
	listOffset = this->FindMenuItem(-1, 1);
	selectedItem = listOffset;
	focus = 0; // allow focus

	trigA = new GuiTrigger;
	trigA->SetSimpleTrigger(-1, WPAD_BUTTON_A | WPAD_CLASSIC_BUTTON_A, PAD_BUTTON_A);

	bgOptions = new GuiImageData(bg_options_png);
	bgOptionsImg = new GuiImage(bgOptions);
	bgOptionsImg->SetParent(this);
	bgOptionsImg->SetAlignment(ALIGN_LEFT, ALIGN_MIDDLE);

	bgOptionsEntry = new GuiImageData(bg_options_entry_png);

	scrollbar = new GuiImageData(scrollbar_png);
	scrollbarImg = new GuiImage(scrollbar);
	scrollbarImg->SetParent(this);
	scrollbarImg->SetAlignment(ALIGN_RIGHT, ALIGN_TOP);
	scrollbarImg->SetPosition(0, 30);

	arrowDown = new GuiImageData(scrollbar_arrowdown_png);
	arrowDownImg = new GuiImage(arrowDown);
	arrowDownOver = new GuiImageData(scrollbar_arrowdown_over_png);
	arrowDownOverImg = new GuiImage(arrowDownOver);
	arrowUp = new GuiImageData(scrollbar_arrowup_png);
	arrowUpImg = new GuiImage(arrowUp);
	arrowUpOver = new GuiImageData(scrollbar_arrowup_over_png);
	arrowUpOverImg = new GuiImage(arrowUpOver);
	scrollbarBox = new GuiImageData(scrollbar_box_png);
	scrollbarBoxImg = new GuiImage(scrollbarBox);
	scrollbarBoxOver = new GuiImageData(scrollbar_box_over_png);
	scrollbarBoxOverImg = new GuiImage(scrollbarBoxOver);

	arrowUpBtn = new GuiButton(arrowUpImg->GetWidth(), arrowUpImg->GetHeight());
	arrowUpBtn->SetParent(this);
	arrowUpBtn->SetImage(arrowUpImg);
	arrowUpBtn->SetImageOver(arrowUpOverImg);
	arrowUpBtn->SetAlignment(ALIGN_RIGHT, ALIGN_TOP);
	arrowUpBtn->SetSelectable(false);
	arrowUpBtn->SetTrigger(trigA);

	arrowDownBtn = new GuiButton(arrowDownImg->GetWidth(), arrowDownImg->GetHeight());
	arrowDownBtn->SetParent(this);
	arrowDownBtn->SetImage(arrowDownImg);
	arrowDownBtn->SetImageOver(arrowDownOverImg);
	arrowDownBtn->SetAlignment(ALIGN_RIGHT, ALIGN_BOTTOM);
	arrowDownBtn->SetSelectable(false);
	arrowDownBtn->SetTrigger(trigA);

	scrollbarBoxBtn = new GuiButton(scrollbarBoxImg->GetWidth(), scrollbarBoxImg->GetHeight());
	scrollbarBoxBtn->SetParent(this);
	scrollbarBoxBtn->SetImage(scrollbarBoxImg);
	scrollbarBoxBtn->SetImageOver(scrollbarBoxOverImg);
	scrollbarBoxBtn->SetAlignment(ALIGN_RIGHT, ALIGN_TOP);
	scrollbarBoxBtn->SetSelectable(false);

	for(int i=0; i<PAGESIZE; i++)
	{
		optionTxt[i] = new GuiText(options->name[i], 22, (GXColor){0, 0, 0, 0xff});
		optionTxt[i]->SetAlignment(ALIGN_LEFT, ALIGN_MIDDLE);
		optionTxt[i]->SetPosition(8,0);

		optionVal[i] = new GuiText(NULL, 22, (GXColor){0, 0, 0, 0xff});
		optionVal[i]->SetAlignment(ALIGN_LEFT, ALIGN_MIDDLE);
		optionVal[i]->SetPosition(300,0);

		optionBg[i] = new GuiImage(bgOptionsEntry);

		optionBtn[i] = new GuiButton(552,30);
		optionBtn[i]->SetParent(this);
		optionBtn[i]->SetLabel(optionTxt[i], 0);
		optionBtn[i]->SetLabel(optionVal[i], 1);
		optionBtn[i]->SetImageOver(optionBg[i]);
		optionBtn[i]->SetPosition(0,30*i+3);
		optionBtn[i]->SetTrigger(trigA);
	}
}

/**
 * Destructor for the GuiOptionBrowser class.
 */
GuiOptionBrowser::~GuiOptionBrowser()
{
	delete arrowUpBtn;
	delete arrowDownBtn;
	delete scrollbarBoxBtn;

	delete bgOptionsImg;
	delete scrollbarImg;
	delete arrowDownImg;
	delete arrowDownOverImg;
	delete arrowUpImg;
	delete arrowUpOverImg;
	delete scrollbarBoxImg;
	delete scrollbarBoxOverImg;

	delete bgOptions;
	delete bgOptionsEntry;
	delete scrollbar;
	delete arrowDown;
	delete arrowDownOver;
	delete arrowUp;
	delete arrowUpOver;
	delete scrollbarBox;
	delete scrollbarBoxOver;

	delete trigA;

	for(int i=0; i<PAGESIZE; i++)
	{
		delete optionTxt[i];
		delete optionVal[i];
		delete optionBg[i];
		delete optionBtn[i];
	}
}

void GuiOptionBrowser::SetFocus(int f)
{
	focus = f;

	for(int i=0; i<PAGESIZE; i++)
		optionBtn[i]->ResetState();

	if(f == 1)
		optionBtn[selectedItem]->SetState(STATE_SELECTED);
}

void GuiOptionBrowser::ResetState()
{
	state = STATE_DEFAULT;

	for(int i=0; i<PAGESIZE; i++)
	{
		optionBtn[i]->ResetState();
	}
}

int GuiOptionBrowser::GetClickedOption()
{
	int found = -1;
	for(int i=0; i<PAGESIZE; i++)
	{
		if(optionBtn[i]->GetState() == STATE_CLICKED)
		{
			optionBtn[i]->SetState(STATE_SELECTED);
			found = optionIndex[i];
			break;
		}
	}
	return found;
}

/****************************************************************************
 * FindMenuItem
 *
 * Help function to find the next visible menu item on the list
 ***************************************************************************/

int GuiOptionBrowser::FindMenuItem(int currentItem, int direction)
{
	int nextItem = currentItem + direction;

	if(nextItem < 0 || nextItem >= options->length)
		return -1;

	if(strlen(options->name[nextItem]) > 0)
		return nextItem;
	else
		return FindMenuItem(nextItem, direction);
}

/**
 * Draw the button on screen
 */
void GuiOptionBrowser::Draw()
{
	if(!this->IsVisible())
		return;

	bgOptionsImg->Draw();

	int next = listOffset;

	for(int i=0; i<PAGESIZE; i++)
	{
		if(next >= 0)
		{
			optionBtn[next]->Draw();
			next = this->FindMenuItem(next, 1);
		}
		else
			break;
	}

	/*scrollbarImg->Draw();
	arrowUpBtn->Draw();
	arrowDownBtn->Draw();
	scrollbarBoxBtn->Draw();*/
}

void GuiOptionBrowser::Update(GuiTrigger * t)
{
	int next, prev;

	// update the location of the scroll box based on the position in the option list
	//int position = 144*(browser.pageIndex + selectedItem) / browser.numEntries;
	//scrollbarBoxBtn->SetPosition(0,position+36);

	arrowUpBtn->Update(t);
	arrowDownBtn->Update(t);
	scrollbarBoxBtn->Update(t);

	// pad/joystick navigation
	if(!focus)
		goto endNavigation; // skip navigation

	if(t->wpad.btns_d & (WPAD_BUTTON_DOWN | WPAD_CLASSIC_BUTTON_DOWN)
		|| t->pad.btns_d & PAD_BUTTON_DOWN)
	{
		next = this->FindMenuItem(selectedItem, 1);

		if(next >= 0)
		{
			if(selectedItem == PAGESIZE-1)
			{
				// move list down by 1
				listOffset = this->FindMenuItem(listOffset, 1);
			}
			else if(optionBtn[selectedItem+1]->IsVisible())
			{
				optionBtn[selectedItem]->ResetState();
				optionBtn[next]->SetState(STATE_SELECTED);
				selectedItem++;
			}
		}
	}
	else if(t->wpad.btns_d & (WPAD_BUTTON_UP | WPAD_CLASSIC_BUTTON_UP)
		|| t->pad.btns_d & PAD_BUTTON_UP)
	{
		prev = this->FindMenuItem(selectedItem, -1);

		if(prev >= 0)
		{
			if(selectedItem == 0)
			{
				// move list up by 1
				listOffset = prev;
			}
			else
			{
				optionBtn[selectedItem]->ResetState();
				optionBtn[prev]->SetState(STATE_SELECTED);
				selectedItem--;
			}
		}
	}

endNavigation:

	next = listOffset;

	for(int i=0; i<PAGESIZE; i++)
	{
		if(next >= 0)
		{
			if(optionBtn[i]->GetState() == STATE_DISABLED)
			{
				optionBtn[i]->SetVisible(true);
				optionBtn[i]->ResetState();
			}

			optionTxt[i]->SetText(options->name[next]);
			optionVal[i]->SetText(options->value[next]);
			optionIndex[i] = next;
		}
		else
		{
			optionBtn[i]->SetVisible(false);
			optionBtn[i]->SetState(STATE_DISABLED);
		}

		optionBtn[i]->Update(t);

		if(optionBtn[i]->GetState() == STATE_SELECTED)
		{
			selectedItem = i;
		}
		next = this->FindMenuItem(next, 1);
	}
}