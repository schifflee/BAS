#include "browsereventsemulator.h"
#include <windows.h>
#include <bitset>
#include "converter.h"
#include "log.h"
#include <math.h>


KeyState::KeyState()
{
    Clear();
}

void KeyState::Clear()
{
    IsShift = false;
    IsAlt = false;
    IsCtrl = false;
}

bool KeyState::IsClear()
{
    return !IsShift && !IsAlt && !IsCtrl;
}


BrowserEventsEmulator::BrowserEventsEmulator()
{

}

void BrowserEventsEmulator::SetFocus(CefRefPtr<CefBrowser> Browser)
{
    if(Browser)
        Browser->GetHost()->SendFocusEvent(true);
}

bool BrowserEventsEmulator::IsPointOnScreen(int PointX, int PointY, int ScrollX, int ScrollY, int BrowserWidth, int BrowserHeight)
{
    worker_log(
                std::string("IsPointOnScreen<<PointX") + std::to_string(PointX)
                + std::string("<<PointY") + std::to_string(PointY)
                + std::string("<<ScrollX") + std::to_string(ScrollX)
                + std::string("<<ScrollY") + std::to_string(ScrollY)
                +std::string("<<BrowserWidth") + std::to_string(BrowserWidth)
                + std::string("<<BrowserHeight") + std::to_string(BrowserHeight)
                );
    return PointX>=ScrollX && PointX<=ScrollX + BrowserWidth && PointY>=ScrollY && PointY<=ScrollY + BrowserHeight;
}

void BrowserEventsEmulator::MouseMove(CefRefPtr<CefBrowser> Browser, bool & IsMouseMoveSimulation, int MouseStartX, int MouseStartY, int MouseEndX, int MouseEndY , int& MouseCurrentX, int& MouseCurrentY, float Speed, int ScrollX, int ScrollY, int BrowserWidth, int BrowserHeight)
{
    if(!IsMouseMoveSimulation)
        return;

    float DistanceSquareAll = (MouseStartX - MouseEndX) * (MouseStartX - MouseEndX) + (MouseStartY - MouseEndY) * (MouseStartY - MouseEndY);
    float DistanceSquareCurrent = (MouseStartX - MouseCurrentX) * (MouseStartX - MouseCurrentX) + (MouseStartY - MouseCurrentY) * (MouseStartY - MouseCurrentY);

    float DistanceAll = sqrtf(DistanceSquareAll);
    float DistanceCurrent = sqrtf(DistanceSquareCurrent);

    DistanceCurrent += Speed;

    if(DistanceCurrent >= DistanceAll)
    {
        IsMouseMoveSimulation = false;
        MouseCurrentX = MouseEndX;
        MouseCurrentY = MouseEndY;
        return;
    }

    float k = DistanceCurrent / (DistanceAll - DistanceCurrent);

    MouseCurrentX = ((float)MouseStartX + k * (float)MouseEndX) / (1.0 + k);
    MouseCurrentY = ((float)MouseStartY + k * (float)MouseEndY) / (1.0 + k);


    if(MouseCurrentX >= ScrollX && MouseCurrentX <= ScrollX + BrowserWidth && MouseCurrentY >= ScrollY && MouseCurrentY <= ScrollY + BrowserHeight)
    {
        CefMouseEvent e;
        e.x = MouseCurrentX - ScrollX;
        e.y = MouseCurrentY - ScrollY;

        Browser->GetHost()->SendMouseMoveEvent(e,false);
    }


}

void BrowserEventsEmulator::MouseClick(CefRefPtr<CefBrowser> Browser, int x, int y, const std::pair<int,int> scroll)
{
    if(!Browser)
        return;

    CefMouseEvent event;
    event.modifiers = EVENTFLAG_LEFT_MOUSE_BUTTON;
    event.x = x - scroll.first;
    event.y = y - scroll.second;
    worker_log(std::string("BrowserEventsEmulator::MouseClick<<") + std::to_string(x) + std::string("<<") + std::to_string(y));
    Browser->GetHost()->SendMouseClickEvent(event,MBT_LEFT,false,1);
    Browser->GetHost()->SendMouseClickEvent(event,MBT_LEFT,true,1);
}

void BrowserEventsEmulator::Key(CefRefPtr<CefBrowser> Browser, std::string & text, KeyState& State)
{
    if(!Browser)
        return;

    bool AdditionalIsShift = false;
    bool AdditionalIsAlt = false;
    bool AdditionalIsCtrl = false;

    std::string text_copy = text;
    while(true)
    {
        if(text_copy.rfind("<SHIFT>", 0) == 0)
        {
            AdditionalIsShift = true;
            text_copy.erase(text_copy.begin(),text_copy.begin() + 7);
            continue;
        }
        if(text_copy.rfind("<MENU>", 0) == 0)
        {
            AdditionalIsAlt = true;
            text_copy.erase(text_copy.begin(),text_copy.begin() + 5);
            continue;
        }
        if(text_copy.rfind("<CONTROL>", 0) == 0)
        {
            AdditionalIsCtrl = true;
            text_copy.erase(text_copy.begin(),text_copy.begin() + 9);
            continue;
        }
        break;
    }

    if(text_copy.empty() && State.IsClear())
        return;

    std::wstring text_whcar = s2ws(text_copy);
    if(text_whcar.empty() && State.IsClear())
        return;

    char key = -1;
    char state = -1;
    wchar_t letter_wchar;
    bool is_special_letter = true;
    int character_length = 1;

    if(!text_whcar.empty())
    {
        std::wstring CANCEL = L"<CANCEL>";
        std::wstring BACK = L"<BACK>";
        std::wstring TAB = L"<TAB>";
        std::wstring CLEAR = L"<CLEAR>";
        std::wstring RETURN = L"<RETURN>";
        std::wstring CAPITAL = L"<CAPITAL>";
        std::wstring ESCAPE = L"<ESCAPE>";
        std::wstring PRIOR = L"<PRIOR>";
        std::wstring NEXT = L"<NEXT>";
        std::wstring END = L"<END>";
        std::wstring HOME = L"<HOME>";
        std::wstring LEFT = L"<LEFT>";
        std::wstring UP = L"<UP>";
        std::wstring RIGHT = L"<RIGHT>";
        std::wstring DOWN = L"<DOWN>";
        std::wstring SELECT = L"<SELECT>";
        std::wstring PRINT = L"<PRINT>";
        std::wstring EXECUTE = L"<EXECUTE>";
        std::wstring SNAPSHOT = L"<SNAPSHOT>";
        std::wstring INSERT = L"<INSERT>";
        std::wstring _DELETE = L"<DELETE>";


        if(text_whcar.rfind(CANCEL, 0) == 0)
        {
            character_length = CANCEL.length();
            letter_wchar = key = VK_CANCEL;
        }else if(text_whcar.rfind(BACK, 0) == 0)
        {
            character_length = BACK.length();
            letter_wchar = key = VK_BACK;
        }else if(text_whcar.rfind(TAB, 0) == 0)
        {
            character_length = TAB.length();
            letter_wchar = key = VK_TAB;
        }else if(text_whcar.rfind(CLEAR, 0) == 0)
        {
            character_length = CLEAR.length();
            letter_wchar = key = VK_CLEAR;
        }else if(text_whcar.rfind(RETURN, 0) == 0)
        {
            character_length = RETURN.length();
            letter_wchar = key = VK_RETURN;
            is_special_letter = false;
        }else if(text_whcar.rfind(CAPITAL, 0) == 0)
        {
            character_length = CAPITAL.length();
            letter_wchar = key = VK_CAPITAL;
        }else if(text_whcar.rfind(ESCAPE, 0) == 0)
        {
            character_length = ESCAPE.length();
            letter_wchar = key = VK_ESCAPE;
        }else if(text_whcar.rfind(PRIOR, 0) == 0)
        {
            character_length = PRIOR.length();
            letter_wchar = key = VK_PRIOR;
        }else if(text_whcar.rfind(NEXT, 0) == 0)
        {
            character_length = NEXT.length();
            letter_wchar = key = VK_NEXT;
        }else if(text_whcar.rfind(END, 0) == 0)
        {
            character_length = END.length();
            letter_wchar = key = VK_END;
        }else if(text_whcar.rfind(HOME, 0) == 0)
        {
            character_length = HOME.length();
            letter_wchar = key = VK_HOME;
        }else if(text_whcar.rfind(LEFT, 0) == 0)
        {
            character_length = LEFT.length();
            letter_wchar = key = VK_LEFT;
        }else if(text_whcar.rfind(UP, 0) == 0)
        {
            character_length = UP.length();
            letter_wchar = key = VK_UP;
        }else if(text_whcar.rfind(RIGHT, 0) == 0)
        {
            character_length = RIGHT.length();
            letter_wchar = key = VK_RIGHT;
        }else if(text_whcar.rfind(DOWN, 0) == 0)
        {
            character_length = DOWN.length();
            letter_wchar = key = VK_DOWN;
        }else if(text_whcar.rfind(SELECT, 0) == 0)
        {
            character_length = SELECT.length();
            letter_wchar = key = VK_SELECT;
        }else if(text_whcar.rfind(PRINT, 0) == 0)
        {
            character_length = PRINT.length();
            letter_wchar = key = VK_PRINT;
        }else if(text_whcar.rfind(EXECUTE, 0) == 0)
        {
            character_length = EXECUTE.length();
            letter_wchar = key = VK_EXECUTE;
        }else if(text_whcar.rfind(SNAPSHOT, 0) == 0)
        {
            character_length = SNAPSHOT.length();
            letter_wchar = key = VK_SNAPSHOT;
        }else if(text_whcar.rfind(INSERT, 0) == 0)
        {
            character_length = INSERT.length();
            letter_wchar = key = VK_INSERT;
        }else if(text_whcar.rfind(_DELETE, 0) == 0)
        {
            character_length = _DELETE.length();
            letter_wchar = key = VK_DELETE;
        }else
        {
            is_special_letter = false;
            letter_wchar = text_whcar.at(0);
            std::vector<HKL> Locales;
            int max_locales = 300;
            Locales.resize(max_locales);
            max_locales = GetKeyboardLayoutList(max_locales,Locales.data());

            int index = 0;
            while((key == -1 || state == -1) && index < max_locales)
            {
                short c = VkKeyScanEx(letter_wchar,Locales[index]);
                key = c & 0xFF;
                state = c >> 8;
                index ++;
            }
        }
    }

    bool IsShift = false;
    bool IsAlt = false;
    bool IsCtrl = false;

    if(state != -1)
    {
        IsShift = state&1;
        IsCtrl = state&2;
        IsAlt = state&4;
    }

    if(AdditionalIsShift)
        IsShift = true;

    if(AdditionalIsAlt)
        IsAlt = true;

    if(AdditionalIsCtrl)
        IsCtrl = true;

    if(IsShift != State.IsShift)
    {
        State.IsShift = IsShift;
        CefKeyEvent event;
        event.type = (IsShift) ? KEYEVENT_KEYDOWN : KEYEVENT_KEYUP;
        event.modifiers = ((State.IsShift) ? EVENTFLAG_SHIFT_DOWN : EVENTFLAG_NONE)  | ((State.IsAlt) ? EVENTFLAG_ALT_DOWN : EVENTFLAG_NONE) | ((State.IsCtrl) ? EVENTFLAG_CONTROL_DOWN : EVENTFLAG_NONE);
        event.windows_key_code = VK_SHIFT;
        event.native_key_code = (IsShift) ? 1 : -1073741823;
        event.is_system_key = false;
        event.character = VK_SHIFT;
        event.unmodified_character = VK_SHIFT;
        event.focus_on_editable_field = true;
        Browser->GetHost()->SendKeyEvent(event);
        return;
    }

    if(IsAlt != State.IsAlt)
    {
        State.IsAlt = IsAlt;
        CefKeyEvent event;
        event.type = (IsAlt) ? KEYEVENT_KEYDOWN : KEYEVENT_KEYUP;
        event.modifiers = ((State.IsShift) ? EVENTFLAG_SHIFT_DOWN : EVENTFLAG_NONE)  | ((State.IsAlt) ? EVENTFLAG_ALT_DOWN : EVENTFLAG_NONE) | ((State.IsCtrl) ? EVENTFLAG_CONTROL_DOWN : EVENTFLAG_NONE);
        event.windows_key_code = VK_MENU;
        event.native_key_code = (IsAlt) ? 1 : -1073741823;
        event.is_system_key = false;
        event.character = VK_MENU;
        event.unmodified_character = VK_MENU;
        event.focus_on_editable_field = true;
        Browser->GetHost()->SendKeyEvent(event);
        return;
    }

    if(IsCtrl != State.IsCtrl)
    {
        State.IsCtrl = IsCtrl;
        CefKeyEvent event;
        event.type = (IsCtrl) ? KEYEVENT_KEYDOWN : KEYEVENT_KEYUP;
        event.modifiers = ((State.IsShift) ? EVENTFLAG_SHIFT_DOWN : EVENTFLAG_NONE)  | ((State.IsAlt) ? EVENTFLAG_ALT_DOWN : EVENTFLAG_NONE) | ((State.IsCtrl) ? EVENTFLAG_CONTROL_DOWN : EVENTFLAG_NONE);
        event.windows_key_code = VK_CONTROL;
        event.native_key_code = (IsCtrl) ? 1 : -1073741823;
        event.is_system_key = false;
        event.character = VK_CONTROL;
        event.unmodified_character = VK_CONTROL;
        event.focus_on_editable_field = true;
        Browser->GetHost()->SendKeyEvent(event);
        return;
    }

    //Main key down
    {
        CefKeyEvent event;
        event.type = KEYEVENT_KEYDOWN;
        event.modifiers = ((IsShift) ? EVENTFLAG_SHIFT_DOWN : EVENTFLAG_NONE)  | ((IsAlt) ? EVENTFLAG_ALT_DOWN : EVENTFLAG_NONE) | ((IsCtrl) ? EVENTFLAG_CONTROL_DOWN : EVENTFLAG_NONE);
        event.windows_key_code = key;
        event.native_key_code = 1;
        event.is_system_key = false;
        event.character = letter_wchar;
        event.unmodified_character = letter_wchar;
        event.focus_on_editable_field = true;
        Browser->GetHost()->SendKeyEvent(event);
    }
    //Main key press
    if(!is_special_letter)
    {
        CefKeyEvent event;
        event.type = KEYEVENT_CHAR;
        event.modifiers = ((IsShift) ? EVENTFLAG_SHIFT_DOWN : EVENTFLAG_NONE)  | ((IsAlt) ? EVENTFLAG_ALT_DOWN : EVENTFLAG_NONE) | ((IsCtrl) ? EVENTFLAG_CONTROL_DOWN : EVENTFLAG_NONE);
        event.windows_key_code = letter_wchar;
        event.native_key_code = 0;
        event.is_system_key = false;
        event.character = letter_wchar;
        event.unmodified_character = letter_wchar;
        event.focus_on_editable_field = true;
        Browser->GetHost()->SendKeyEvent(event);
    }
    //Main key up
    {
        CefKeyEvent event;
        event.type = KEYEVENT_KEYUP;
        event.modifiers = ((IsShift) ? EVENTFLAG_SHIFT_DOWN : EVENTFLAG_NONE)  | ((IsAlt) ? EVENTFLAG_ALT_DOWN : EVENTFLAG_NONE) | ((IsCtrl) ? EVENTFLAG_CONTROL_DOWN : EVENTFLAG_NONE);
        event.windows_key_code = key;
        event.native_key_code = -1073741823;
        event.is_system_key = false;
        event.character = letter_wchar;
        event.unmodified_character = letter_wchar;
        event.focus_on_editable_field = true;
        Browser->GetHost()->SendKeyEvent(event);
    }

    text_whcar.erase(text_whcar.begin(),text_whcar.begin() + character_length);
    text = ws2s(text_whcar);
}

