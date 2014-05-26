#include "SFApp.h"

SFApp::SFApp() : fire(0), score(0), is_running(true) {

  surface = SDL_GetVideoSurface();
  app_box = make_shared<SFBoundingBox>(Vector2(surface->w/2, surface->h/2), surface->w/2, surface->h/2);
  player  = make_shared<SFAsset>(SFASSET_PLAYER);
  auto player_pos = Point2(surface->w/2, 88.0f);
  player->SetPosition(player_pos);

  const int number_of_aliens = 10;
  int height = 200.0f;
  for(int i=0; i<3;i++){
	  for(int i=0; i<number_of_aliens; i++) {
	    // place an alien at width/number_of_aliens * i
	    auto alien = make_shared<SFAsset>(SFASSET_ALIEN);
	    auto pos   = Point2(20+(surface->w/number_of_aliens) * i, height);
	    alien->SetPosition(pos);
	    aliens.push_back(alien);
	  }
  height += 60.0f;
  }

  auto coin = make_shared<SFAsset>(SFASSET_COIN);
  auto pos  = Point2((surface->w/4), 450);
  coin->SetPosition(pos);
  coins.push_back(coin);
}

SFApp::~SFApp() {
}

/**
 * Handle all events that come from SDL.
 * These are timer or keyboard events.
 */
void SFApp::OnEvent(SFEvent& event) {
  SFEVENT the_event = event.GetCode();
  switch (the_event) {
  case SFEVENT_QUIT:
    is_running = false;
    break;
  case SFEVENT_UPDATE:
    OnUpdateWorld();
    OnRender();
    break;
  case SFEVENT_PLAYER_LEFT:
    player->GoWest();
    break;
  case SFEVENT_PLAYER_RIGHT:
    player->GoEast();
    break;
  case SFEVENT_FIRE:
    fire ++;
    std::stringstream sstm;
    sstm << "Fire " << fire;
    SDL_WM_SetCaption(sstm.str().c_str(),  sstm.str().c_str());
    FireProjectile();
    break;
  }
}

int SFApp::OnExecute() {
  // Execute the app
  SDL_Event event;
  while (SDL_WaitEvent(&event) && is_running) {
    // if this is an update event, then handle it in SFApp,
    // otherwise punt it to the SFEventDispacher.
    SFEvent sfevent((const SDL_Event) event);
    OnEvent(sfevent);
  }
}

void SFApp::OnUpdateWorld() {
  // Update projectile positions
  for(auto p: projectiles) {
    p->GoNorth();
  }

//
  for(auto c: coins) {
    c->CoinMove();
  }

  // Update enemy positions
  for(auto a : aliens) {
    // do something here
  }

  // Detect collisions
  for(auto p : projectiles) {
    for(auto a : aliens) {
      if(p->CollidesWith(a)) {
	score ++;
        p->HandleCollision();
        a->HandleCollision();
      }
    }
  }

  for(auto p: projectiles) {
    for(auto c : coins) {
      if(p->CollidesWith(c)) {
	score += 10;
	p->HandleCollision();
	c->HandleCollision();
      }
    }
  }

  if(score == 40) {
	cout << "Well done you scored : " << score << endl;
	exit(0);
  }

  // remove dead aliens (the long way)
  list<shared_ptr<SFAsset>> tmp;
  for(auto a : aliens) {
    if(a->IsAlive()) {
      tmp.push_back(a);
    }
  }
  aliens.clear();
  aliens = list<shared_ptr<SFAsset>>(tmp);

  list<shared_ptr<SFAsset>> tmp1;
  for(auto p : projectiles) {
    if(p->IsAlive()) {
	tmp1.push_back(p);
    }
  }
  projectiles.clear();
  projectiles = list<shared_ptr<SFAsset>>(tmp1);

  list<shared_ptr<SFAsset>> tmp2;
  for(auto c : coins) {
    if(c->IsAlive()) {
	tmp2.push_back(c);
    }
  }
  coins.clear();
  coins = list<shared_ptr<SFAsset>>(tmp2);
}

void SFApp::OnRender() {
  // clear the surface
  SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 8, 54, 129) );

  // draw the player
  player->OnRender(surface);

  for(auto p: projectiles) {
    if(p->IsAlive()) {p->OnRender(surface);}
  }

  for(auto a: aliens) {
    if(a->IsAlive()) {a->OnRender(surface);}
  }

  for(auto c: coins) {
    c->OnRender(surface);
  }

  // Switch the off-screen buffer to be on-screen
  SDL_Flip(surface);
}

void SFApp::FireProjectile() {
  auto pb = make_shared<SFAsset>(SFASSET_PROJECTILE);
  auto v  = player->GetPosition();
  pb->SetPosition(v);
  projectiles.push_back(pb);
}
