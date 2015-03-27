package modularelectronics.modularelectronics.ModuleFragment;

import android.graphics.drawable.Drawable;

public class ListViewItem {
    public Drawable icon;
    public String title;
    public String description;

    public ListViewItem(Drawable icon, String title, String description){
        this.icon = icon;
        this.title = title;
        this.description = description;
    }

    public void updateIcon(Drawable icon){
        this.icon=icon;
    }
    public void updateTitle(String title){
        this.title = title;
    }
    public void updateDescription(String description){
        this.description = description;
    }
}
