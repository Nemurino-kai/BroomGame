# include <Siv3D.hpp>
# include <HamFramework.hpp>
# include "gameManager.h"

namespace GameInfo
{
	// ゲームのタイトル
	const String Title = L"ほうきを\n倒さない\nゲーム";

	// ゲームのバージョン（空の文字列も OK）
	const String Version = L"";

	// ゲームの Web サイト（無い場合は空の文字列にする）
	const String WebURL = L"";

	// 結果ツイートの文章（TweetHead + score + TweetTail)
	const String TweetHead = L"Biscuit Clicker をプレイしたよ。結果: ";

	const String TweetTail = L" 枚 #BiscuitClicker";

	// ゲームの背景色
	//const ColorF BackgroundColor = ColorF(0.4, 0.7, 0.5);
	const ColorF BackgroundColor = Color(L"#99CC00");


	// シーン切り替え時のフェードイン・アウトの色
	const ColorF FadeInColor = ColorF(1.0, 1.0, 1.0);

	// タイトルのフォントサイズ
	const int32 TitleFontSize = 36;

	// メニューのフォントサイズ
	const int32 MenuFontSize = 22;

	// メニュー項目「ゲームスタート」
	const String MenuGameStart = L"はじめる";

	// メニュー項目「スコア」
	const String MenuScore = L"とくてん";

	// メニュー項目「クレジット」
	const String MenuCredit = L"くれじっと";

	// メニュー項目「Web サイト」
	const String MenuWebURL = L"Web サイト";

	// メニュー項目「終了」
	const String MenuExit = L"おわり";

	// セーブデータの保存場所
	const FilePath SaveFilePath = L"Save/Score.dat";

	// スタッフロールのクレジット（項目は増減できる）
	const Array<std::pair<String, Array<String>>> Credits
	{
		{ L"使用ライブラリ",{ L"Siv3D" } },
		{ L"背景",{ L"きまぐれアフター 様" } },
		{ L"BGM",{ L"甘茶の音楽工房 様" } },
		{ L"効果音",{ L"効果音ラボ 様" } },
		{ L"フォント",{ L"たぬき油性マジック",L"たぬき侍 様" } },
		{ L"プログラマ",{ L"DoubleZ" } },
		{ L"腕",{ L"DoubleZ" } },
		{ L"Special Thanks",{ L"You!" } },
	};
}


///////////////////////////////////////////////////////////////////////
//
//  タイトル画面のメニュー選択時のエフェクト
//
struct MenuEffect : IEffect
{
	RoundRect m_rect;

	MenuEffect(const RoundRect& rect)
		: m_rect(rect) {}

	bool update(double timeSec)
	{
		const double e = EaseOut<Easing::Quad>(timeSec);
		RoundRect(m_rect).stretched(e * 20).draw(AlphaF((1.0 - e) * 0.4));
		return timeSec < 1.0;
	}
};

///////////////////////////////////////////////////////////////////////
//
//  ビスケットの描画
//
void DrawBiscuit(const Circle& c, bool drawShadow)
{
	c.drawShadow({ 0, 2 }, 12, 2 + drawShadow * 2)
		.draw(Color(250, 180, 100))
		.drawFrame(1, 1, Color(240, 140, 80));

}

///////////////////////////////////////////////////////////////////////
//
//  全てのシーンからアクセスできるデータ
//
struct GameData
{
	double lastScore = 0;
};

using MyApp = SceneManager<String, GameData>;

///////////////////////////////////////////////////////////////////////
//
//  セーブ用スコアデータ
//
struct ScoreData
{
	double score;

	Date date;

	template <class Archive>
	void serialize(Archive& archive)
	{
		archive(score, date);
	}
};

///////////////////////////////////////////////////////////////////////
//
//  デフォルトのハイスコア
//
const std::array<ScoreData, 5> defaultHighScores
{
	ScoreData{ 30.00, Date(2019,1,1) },
	ScoreData{ 20.00, Date(2019,1,1) },
	ScoreData{ 10.00, Date(2019,1,1) },
	ScoreData{ 5.00, Date(2019,1,1) },
	ScoreData{ 1.00, Date(2019,1,1) },
};

///////////////////////////////////////////////////////////////////////
//
//  タイトル画面
//
class Title : public MyApp::Scene
{
private:

	Effect m_effect;
	
	Stopwatch m_effectMenuItemStopwatch{ true };

	Array<RoundRect> m_menuBoxes;

	Array<String> m_menuTexts =
	{
		GameInfo::MenuGameStart,
		GameInfo::MenuScore,
		GameInfo::MenuCredit,
		GameInfo::MenuWebURL,
		GameInfo::MenuExit
	};

public:

	~Title()
	{
		Cursor::SetStyle(CursorStyle::Default);
	}

	void init() override
	{

		if (GameInfo::WebURL.isEmpty)
		{
			m_menuTexts.erase(m_menuTexts.begin() + 3);
		}

		m_menuBoxes.resize(m_menuTexts.size());

		int32 boxWidth = 0;

		for (const auto& text : m_menuTexts)
		{
			boxWidth = Max(boxWidth, FontAsset(L"Menu")(text).region().w);
		}

		for (auto i : step(m_menuBoxes.size()))
		{
			m_menuBoxes[i].set(200-(boxWidth+80)/2, 300 + i * 70, boxWidth + 80, 50,10);
		}
	}

	void update() override
	{
		bool handCursor = false;

		for (auto i : step(m_menuBoxes.size()))
		{
			const RoundRect item = m_menuBoxes[i];

			handCursor |= item.mouseOver;

			if (item.mouseOver && m_effectMenuItemStopwatch.elapsed() > 300ms)
			{
				m_effect.add<MenuEffect>(m_menuBoxes[i]);

				m_effectMenuItemStopwatch.restart();
			}

			if (item.leftClicked)
			{
				SoundAsset(L"button").play();
				if (i == 0)
				{
					changeScene(L"Game");
				}
				else if (i == 1)
				{
					changeScene(L"Score");
				}
				else if (i == 2)
				{
					changeScene(L"Credit");
				}
				else if (!GameInfo::WebURL.isEmpty && i == 3)
				{
					Internet::LaunchWebBrowser(GameInfo::WebURL);
				}
				else
				{
					System::Exit();
				}

				break;
			}
		}


		Cursor::SetStyle(handCursor ? CursorStyle::Hand : CursorStyle::Default);
	}

	void draw() const override
	{
		TextureAsset(L"classroom").draw();

		Graphics2D::SetBlendState(BlendState::Additive);

		m_effect.update();

		Graphics2D::SetBlendState(BlendState::Default);

		const double titleHeight = FontAsset(L"Title")(GameInfo::Title).region().h;

		FontAsset(L"Title")(GameInfo::Title).drawAt(Window::Center().x, titleHeight-60);

		for (auto i : step(m_menuBoxes.size()))
		{
			m_menuBoxes[i].draw(HSV(60 * i, 0.3, 1));

			FontAsset(L"Menu")(m_menuTexts[i]).drawAt(m_menuBoxes[i].center, Color(40));
		}

		const Size versionSize = FontAsset(L"Version")(GameInfo::Version).region().size;

		FontAsset(L"Version")(GameInfo::Version).drawAt(Window::Size().moveBy(-versionSize));
	}
};

///////////////////////////////////////////////////////////////////////
//
//  ゲーム
//
class Game : public MyApp::Scene
{
private:
		
	std::unique_ptr<gameManager> game;

public:

	~Game()
	{
		Cursor::SetStyle(CursorStyle::Default);
	}

	void init() override
	{
		game = std::make_unique<gameManager>();
		m_data->lastScore = 0;
	}

	void update() override
	{
		game->update();

		if (game->isOver()) {
			m_data->lastScore = game->getTime();
			changeScene(L"Result", 2000);
		}
	}

	void draw() const override
	{
		game->draw();
	}
};

///////////////////////////////////////////////////////////////////////
//
//  結果画面
//
class Result : public MyApp::Scene
{
private:

	std::array<ScoreData, 5> m_highScores = defaultHighScores;

	const Circle titleButton = Circle(Window::Center().x, Window::Height() * 0.7, 25);
	
	bool rankIn = false,highScore=false;

public:

	~Result()
	{
		Cursor::SetStyle(CursorStyle::Default);
	}

	void init() override
	{
		if (FileSystem::Exists(GameInfo::SaveFilePath))
		{
			Deserializer<BinaryReader>{GameInfo::SaveFilePath}(m_highScores);
		}
		else
		{
			Serializer<BinaryWriter>{GameInfo::SaveFilePath}(m_highScores);
		}

		if (m_highScores.back().score <= m_data->lastScore)
		{
			m_highScores.back() = { m_data->lastScore, Date::Today() };
			rankIn = true;

			std::sort(m_highScores.begin(), m_highScores.end(), [](const ScoreData& a, const ScoreData& b)
			{
				return a.score > b.score ? true : a.score == b.score ? a.date > b.date : false;
			});

			//更新後のハイスコアが直近のスコアか判定
			if (m_highScores.front().score == m_data->lastScore) {
				highScore = true;
			}

			Serializer<BinaryWriter>{GameInfo::SaveFilePath}(m_highScores);
		}
	}

	void update() override
	{
		if (titleButton.leftClicked || Input::KeyEscape.clicked)
		{
			SoundAsset(L"button").play();
			changeScene(L"Title");
		}

		const bool handCursor = titleButton.mouseOver;

		Cursor::SetStyle(handCursor ? CursorStyle::Hand : CursorStyle::Default);
	}

	void draw() const override
	{
		

		const double resultHeight = FontAsset(L"Result")(m_data->lastScore,L"s").region().h;

		FontAsset(L"Result")(m_data->lastScore, L"s").drawAt(Window::Center().x, Window::Height() * 0.5 - resultHeight / 2);

		DrawBiscuit(titleButton, titleButton.mouseOver);
		FontAsset(L"ResultButton")(L"タイトルへ").drawAt(titleButton.center.movedBy(0, 90));

		if (highScore) {
			FontAsset(L"RecordNotification")(L"HighScore!!").drawAt(Window::Center().x,310, Color(L"#FFcccc"));
		}
		else if (rankIn) {
			FontAsset(L"RecordNotification")(L"RankIn!").drawAt(Window::Center().x, 310, Color(L"#FFdf8a"));
				//Color(L"#3300CC"));
		}


	}
};

///////////////////////////////////////////////////////////////////////
//
//  ハイスコア一覧
//
class Score : public MyApp::Scene
{
private:

	std::array<ScoreData, 5> m_highScores = defaultHighScores;

public:

	void init() override
	{
		if (FileSystem::Exists(GameInfo::SaveFilePath))
		{
			Deserializer<BinaryReader>{GameInfo::SaveFilePath}(m_highScores);
		}
		else
		{
			Serializer<BinaryWriter>{GameInfo::SaveFilePath}(m_highScores);
		}
	}

	void update() override
	{
		if ((Input::MouseL | Input::KeyEscape).clicked)
		{
			SoundAsset(L"button").play();
			changeScene(L"Title");
		}
	}

	void draw() const override
	{
		const int32 h = FontAsset(L"ScoreList").height;
		TextureAsset(L"classroom").draw();

		FontAsset(L"Title")(L"らんきんぐ").drawAt(Window::Center().x, 50);
		for (auto i : step(m_highScores.size()))
		{
			const Rect rect = Rect(360, 70).setCenter(Window::Center().x, 150 + i * 90);

			rect.draw(ColorF(HSV(40 * i, 0.8, 1), 0.8));

			FontAsset(L"ScoreList")(m_highScores[i].score)
				.draw(rect.pos + Point(32-10, (rect.h - h) / 2 + 2), Palette::Gray);

			FontAsset(L"ScoreList")(m_highScores[i].score)
				.draw(rect.pos + Point(30-10, (rect.h - h) / 2));

			const Size dateSize = FontAsset(L"ScoreListDate")(m_highScores[i].date).region().size;

			FontAsset(L"ScoreListDate")(m_highScores[i].date)
				.draw(rect.br.x - dateSize.x - 20, rect.center.y - dateSize.y / 2);

		}
	}
};

///////////////////////////////////////////////////////////////////////
//
//  スタッフクレジット
//
class Credit : public MyApp::Scene
{
private:

	Array<std::tuple<String, double, bool>> m_credits;

	double m_height = 0.0;

	Stopwatch m_stopwatch{ true };

	double yOffset() const
	{
		return Window::Height() + 60 - m_stopwatch.ms() / 20.0;
	}

public:

	void init()
	{
		double y = 0;

		for (const auto& credit : GameInfo::Credits)
		{
			m_credits.emplace_back(credit.first, y, true);
			y += 70;

			for (const auto& name : credit.second)
			{
				m_credits.emplace_back(name, y, false);
				y += 60;
			}

			y += 60;
		}

		m_height = y;
	}

	void update() override
	{
		if ((Input::MouseL | Input::KeyEscape).clicked)
		{
			changeScene(L"Title");
		}

		if ((m_height + yOffset()) < 0)
		{
			m_stopwatch.restart();
		}
	}

	void draw() const override
	{
		const double offset = yOffset();

		for (const auto& credit : m_credits)
		{
			FontAsset(std::get<bool>(credit) ? L"CreditBig" : L"CreditSmall")(std::get<String>(credit))
				.drawAt(Window::Center().x, std::get<double>(credit) + offset);
		}
	}
};

///////////////////////////////////////////////////////////////////////
//
//  メイン関数
//
void Main()
{
	Window::Resize(400, 600);
	Window::SetTitle(GameInfo::Title);
	Graphics::SetBackground(GameInfo::BackgroundColor);
	System::SetExitEvent(WindowEvent::CloseButton);

	//////////////////////////////////////////////////////////////
	//
	//  使用するシーン
	//
	MyApp manager;
	manager.setFadeColor(GameInfo::FadeInColor);
	manager.add<Title>(L"Title");
	manager.add<Game>(L"Game");
	manager.add<Result>(L"Result");
	manager.add<Score>(L"Score");
	manager.add<Credit>(L"Credit");

	//////////////////////////////////////////////////////////////
	//
	//  使用するフォント
	//
	FontManager::Register(L"TanukiMagic.ttf");

	FontAsset::Register(L"Title", GameInfo::TitleFontSize, L"たぬき油性マジック", FontStyle::Outline);
	FontAsset(L"Title").changeOutlineStyle(TextOutlineStyle(Color(60), Color(255), GameInfo::TitleFontSize * 0.05));
	FontAsset::Register(L"Menu", GameInfo::MenuFontSize, L"たぬき油性マジック");
	FontAsset::Register(L"Version", 14, Typeface::Regular);
	FontAsset::Register(L"Result", 72, Typeface::Bold);
	FontAsset::Register(L"ResultButton", 32, L"たぬき油性マジック");
	FontAsset::Register(L"GameTime", 40, Typeface::Light);
	FontAsset::Register(L"ScoreList", 32, Typeface::Heavy);
	FontAsset::Register(L"RecordNotification", 28, Typeface::Black);
	FontAsset::Register(L"ScoreListDate", 16, Typeface::Regular, FontStyle::Italic);
	FontAsset::Register(L"CreditBig", 32, Typeface::Bold);
	FontAsset::Register(L"CreditSmall", 20, Typeface::Regular);

	// 使用するテクスチャ
	TextureAsset::Register(L"classroom", L"教室400.jpg");
	TextureAsset::Register(L"arm", L"腕.png");
	TextureAsset::Register(L"broom", L"箒.png");
	TextureAsset::Register(L"baseball", L"yakyu30.png");
	TextureAsset::Register(L"soccer",L"soccer62.png");
	TextureAsset::Register(L"basket", L"basket75.png");


	// 使用するサウンド
	SoundAsset::Register(L"button", L"decision3.mp3");
	SoundAsset::Register(L"bgm", L"lovelyflower.mp3");

	// ループ再生するよう設定
	SoundAsset(L"bgm").setLoop(true);
	SoundAsset(L"bgm").play();

	//////////////////////////////////////////////////////////////
	//
	//  メインループ
	//
	while (System::Update())
	{
		if (!manager.updateAndDraw())
		{
			break;
		}
	}
}
