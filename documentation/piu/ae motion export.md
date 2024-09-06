# Adobe After Effectsを使用してModdableモーションデータをエクスポートする
Copyright 2017 Moddable Tech, Inc.<BR>
改訂： 2017年9月26日

## はじめに

Adobe After Effectsを使用して、モーションデータをModdableのサンプルアプリにエクスポートし、開発に使用することができます。

Moddableのモーションコントロールは位置のみをサポートしています。

AEでは、アセットを持つレイヤーのみがエクスポートされ、位置のみがエクスポートされます。スケーリング、回転、透明度などはエクスポートされません。

Piuは配列内の値の間を線形補間します。After Effectsでフレームレートを下げることで、配列のサイズを小さくすることができます。Piuはモーションエフェクトの定義された時間長に基づいて値の間をアニメーション化します。

### After Effectsにエクスポートスクリプトをインストールする

ae2piu.jsxスクリプトをAfter Effectsアプリケーションフォルダ内の「scripts」フォルダにインストールします。

<img src="../assets/ae-motion/ae_script_install.png" height="233"/>




### モーションデータをエクスポートする

スクリプトを実行するには、File -> Scriptsメニューから「ae2piu.jsx」を選択します。

<img src="../assets/ae-motion//ae2piu-screen.png" width="600"/>

After Effectsでエクスポートスクリプトを実行すると、フォルダを選択するためのダイアログボックスが表示されます。最初に新しいフォルダを作成してください。エクスポートスクリプトはアセットをコピーし、完全なModdableアプリケーションのためのmanifest.jsonとmain.jsファイルを作成します。プロジェクトをビルドしてModdable Simulatorで実行するには、以下のコマンドを使用します。

	cd <選択したフォルダ>
	mcconfig -m


### 注釈

アニメーション全体の長さはAEコンポジションから取得されます。

時間値はミリ秒単位でエクスポートされます。

AEのイージングはエクスポートされませんが、Moddableのイージングをエクスポートされたコードに追加することができます。

例： (nullをMath.quadEaseOutに置き換え）

	this.steps(ball, { x:ballX, y:ballY }, 2333, null, 0, 0);

	this.steps(ball, { x:ballX, y:ballY }, 2333, Math.quadEaseOut, 0, 0);



