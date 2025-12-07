package com.qiplat.sweeteditor.demo;

import android.os.Bundle;

import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;
import com.qiplat.sweeteditor.SweetEditor;

public class MainActivity extends AppCompatActivity {
   private SweetEditor sweetEditor;

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        sweetEditor = findViewById(R.id.editor);
    }
}
