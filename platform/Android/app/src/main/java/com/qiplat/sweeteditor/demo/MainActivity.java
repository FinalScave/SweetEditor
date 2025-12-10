package com.qiplat.sweeteditor.demo;

import android.os.Bundle;
import android.widget.TextView;

import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;
import com.qiplat.sweeteditor.SweetEditor;
import com.qiplat.sweeteditor.core.Document;

public class MainActivity extends AppCompatActivity {
   private SweetEditor sweetEditor;

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        sweetEditor = findViewById(R.id.editor);
        Document document = new Document("AAAABBB\njhhuihii");
        sweetEditor.loadDocument(document);
        TextView textView;
        textView.setLineSpacing();
    }
}
