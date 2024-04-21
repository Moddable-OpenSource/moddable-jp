# Moddable 日本語翻訳プロジェクト

このリポジトリでは [Moddable](https://github.com/Moddable-OpenSource/moddable) が提供するドキュメントを日本語に翻訳する作業を行っています。

## ゴール

[Moddable](https://github.com/Moddable-OpenSource/moddable) が提供するドキュメントを段階的に、いずれ網羅的に翻訳して、日本の開発者がModdableを使いやすくすることを目指します。

## 方針

- 翻訳は完全性よりも速度を重視し、まずはドキュメント全体を一通り翻訳することを目指します
- リポジトリに翻訳を取り込む際は、1名以上の日本語話者によるレビューを行い、翻訳の質を担保します
- 詳しい進め方は作業を行いながら話し合って決めます
    - 進め方について提案がある方は、お気軽に [issue](https://github.com/Moddable-OpenSource/moddable-jp/issues) を立ててみてください

## プロジェクトへの参加

- 本プロジェクトに参加するには、Githubのアカウントが必要です
- 「翻訳者」または「レビュアー」として参加できます
    - 翻訳者：後述の「翻訳作業の流れ」に則ってプルリクエストを作成してください。**初回のプルリクエストを送る際は、[Contributor License Agreement（CLA）](https://github.com/Moddable-OpenSource/moddable/tree/public/licenses#contributor-license-agreement)へのサインが必要です**
    - レビュアー：日本語能力とModdableの知識が十分にあると認められる方はレビュアーとして参加できます。[ししかわ](https://github.com/meganetaaan)まで連絡してください

## 翻訳作業の流れ

- [issue](https://github.com/Moddable-OpenSource/moddable-jp/issues/3) で翻訳する文書を宣言します
- ドキュメントを翻訳します
    - 翻訳作業の中心である `dev/translate-jp` から作業ブランチを切ります
    - リポジトリに含まれる英語文書を翻訳し、同じファイル名で **上書きしてコミット** することで日本語訳を作成します
- 文章校正を行います
    - `textlint` を用いて文法や用語の表記ゆれを自動チェックします
    - 次のコマンドでチェックを行います。指摘がある場合、修正します

```
cd textlint
npm install #初回のみ
npm run textlint
```

- プルリクエストを作成します
    - 1名以上のレビュアーがレビューして修正依頼または承認を行います
    - プルリクエストをマージします

### 公式リポジトリの更新に追従する

- 公式リポジトリに更新があった場合の翻訳作業も基本的には前述の流れで行います。
- 公式リポジトリの翻訳したいブランチを、moddable-jpの作業ブランチにマージします。
    - 公式リポジトリの変更差分がコンフリクトとして現れるので、差分を翻訳してマージコミットを作成します。

## Discordサーバ

- 翻訳の進め方に関する相談、提案は [issue](https://github.com/Moddable-OpenSource/moddable-jp/issues/) で行います
- issueを立てるまでもないカジュアルなやりとりなど、プロジェクトメンバー間のコミュニケーションには、 Discordサーバーを使用しています
- [Discordサーバーの招待リンク](https://discord.gg/7vT4Mde9u2)から参加してください

## 自動翻訳

ChatGPTなどのAIを使った自動翻訳を利用できます。
[`ChatGPT Markdown Translator`](https://github.com/smikitky/chatgpt-md-translator)を使うとマークダウンの文書単位で自動翻訳にかけることができます。
ツールの設定例（`.env`と`prompt.md`）を[`chatgpt-md-translator`](./chatgpt-md-translator/)に置いていますので活用してください。

### 注意点

- `ChatGPT Markdown Translator`を使う際、出力ファイルのオプション（`-o`）を与えない場合は、**元ファイルを上書きする**挙動になるので注意してください
- 出力される翻訳のクオリティは**80点程度**であり、誤訳や用語の表記ゆれを多く含みます。プルリクエストを作成する前に、**必ず全文を自身でチェックしてください**