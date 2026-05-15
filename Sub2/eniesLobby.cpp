#include "eniesLobby.h"

static int clampInt(int value, int lo, int hi) {
    if (value < lo) return lo;
    if (value > hi) return hi;
    return value;
}

static int ceilMul(int value, double ratio) {
    return (int)ceil(value * ratio);
}

static void clampContext(BattleContext& context) {
    context.morale = clampInt(context.morale, 0, 100);
    context.alarmLevel = clampInt(context.alarmLevel, 0, 100);
    context.rescueProgress = clampInt(context.rescueProgress, 0, 100);
    context.escapeProgress = clampInt(context.escapeProgress, 0, 100);
    if (context.busterCallTimer < 0) context.busterCallTimer = 0;
}

static int dealCharacterDamage(Character* target, int rawDamage) {
    if (!target || !target->isAlive()) return 0;
    int effective = rawDamage - target->getDEF();
    if (effective < 0) effective = 0;
    target->receiveDamage(rawDamage);
    return effective;
}

static int dealBuildingDamage(Building* target, int rawDamage) {
    if (!target || target->isDestroyed()) return 0;
    int effective = rawDamage;
    if (effective < 0) effective = 0;
    target->receiveDamage(rawDamage);
    return effective;
}

/*
 * BattleContext
 */
BattleContext::BattleContext() {
    turnCount = 0;
    morale = 0;
    alarmLevel = 0;
    rescueProgress = 0;
    escapeProgress = 0;
    busterCallTimer = 0;
    mainGateDestroyed = false;
    robinRescued = false;
    bridgeOpened = false;
    battleEnded = false;
    resultCode = "";
    strawHats = NULL;
    strawHatCount = 0;
}

void BattleContext::nextTurn() {
    turnCount++;
    clampContext(*this);
}

/*
 * Character
 */
Character::Character() {
    name = "";
    hp = 0;
    maxHp = 0;
    atk = 0;
    def = 0;
    speed = 0;
    energy = 0;
    alive = false;
    lastKnockout = false;
}

Character::Character(string name, int hp, int atk, int def, int speed, int energy) {
    this->name = name;
    this->hp = hp;
    this->maxHp = hp;
    this->atk = atk;
    this->def = def;
    this->speed = speed;
    this->energy = clampInt(energy, 0, 100);
    this->alive = (hp > 0);
    this->lastKnockout = false;
    if (this->hp < 0) this->hp = 0;
    if (this->maxHp < 0) this->maxHp = 0;
}

Character::~Character() {
    return;
}

int Character::attack(Building* target, BattleContext& context) {
    return 0;
}

int Character::specialSkill(Building* target, BattleContext& context) {
    return 0;
}

void Character::endTurn(BattleContext& context) {
    return ;
}

void Character::receiveDamage(int damage) {
    int taken = damage - def;
    if (taken < 0) taken = 0;
    hp -= taken;
    if (hp <= 0) {
        hp = 0;
        alive = false;
    }
}

bool Character::isAlive() const {
    return alive;
}

string Character::getName() const {
    return name;
}

int Character::getHP() const {
    return hp;
}

int Character::getEnergy() const {
    return energy;
}

int Character::getMaxHP() const { return maxHp; }
int Character::getATK() const { return atk; }
int Character::getDEF() const { return def; }
int Character::getSpeed() const { return speed; }
void Character::consumeEnergy(int amount) { energy = clampInt(energy - amount, 0, 100); }
void Character::restoreEnergy(int amount) { energy = clampInt(energy + amount, 0, 100); }
void Character::heal(int amount) {
    hp = clampInt(hp + amount, 0, maxHp);
    alive = (hp > 0);
}
void Character::modifyATK(int delta) { atk = clampInt(atk + delta, 0, INT_MAX); }
void Character::modifyDEF(int delta) { def = clampInt(def + delta, 0, INT_MAX); }
void Character::modifySpeed(int delta) { speed = clampInt(speed + delta, 0, INT_MAX); }
void Character::setLastKnockout(bool value) { lastKnockout = value; }
bool Character::didLastKnockout() const { return lastKnockout; }

bool Character::isStrawHat() const {
    return false;
}

bool Character::isCP9() const {
    return false;
}

/*
 * StrawHat
 */
StrawHat::StrawHat() : Character() {
    bounty = 0;
}

StrawHat::StrawHat(string name, int hp, int atk, int def,
                   int speed, int energy, long long bounty) {
    this->name = name;
    this->hp = hp;
    this->maxHp = hp;
    this->atk = atk;
    this->def = def;
    this->speed = speed;
    this->energy = clampInt(energy, 0, 100);
    this->alive = (hp > 0);
    this->lastKnockout = false;
    if (this->hp < 0) this->hp = 0;
    if (this->maxHp < 0) this->maxHp = 0;
    this->bounty = bounty;
}

bool StrawHat::isStrawHat() const {
    return true;
}

string StrawHat::str() const {
    stringstream ss;
    ss << "StrawHat[name=" << name
       << ", hp=" << hp
       << ", atk=" << atk
       << ", def=" << def
       << ", speed=" << speed
       << ", energy=" << energy
       << ", bounty=" << bounty << "]";
    return ss.str();
}

/*
 * Luffy
 */
Luffy::Luffy(string name, int hp, int atk, int def,
             int speed, int energy, long long bounty)
    : StrawHat(name, hp, atk, def, speed, energy, bounty) {}

int Luffy::attack(Character* target, BattleContext& context) {
    if (!target || !isAlive()) return 0;
    double ratio = (maxHp > 0) ? (double)hp / maxHp : 0.0;
    int raw = atk;
    if (ratio <= 0.3) raw = ceilMul(atk, 1.3);
    else if (ratio <= 0.5) raw = ceilMul(atk, 1.15);
    bool wasAlive = target->isAlive();
    int dealt = dealCharacterDamage(target, raw);
    if (wasAlive && !target->isAlive()) context.morale += 5;
    clampContext(context);
    return dealt;
}

int Luffy::specialSkill(Character* target, BattleContext& context) {
    if (!target || energy < 20 || hp < ceilMul(maxHp, 0.15)) return 0;
    consumeEnergy(20);
    int raw = ceilMul(atk, 2.0);
    bool wasAlive = target->isAlive();
    int dealt = dealCharacterDamage(target, raw);
    modifySpeed(15);
    modifyATK(15);
    context.alarmLevel += 10;
    receiveDamage(ceilMul(maxHp, 0.08) + def);
    if (wasAlive && !target->isAlive()) context.morale += 5;
    clampContext(context);
    return dealt;
}

int Luffy::attack(Building* target, BattleContext& context) {
    if (!target || !isAlive()) return 0;
    double ratio = (maxHp > 0) ? (double)hp / maxHp : 0.0;
    int raw = atk;
    if (ratio <= 0.3) raw = ceilMul(atk, 1.3);
    else if (ratio <= 0.5) raw = ceilMul(atk, 1.15);
    int dealt = dealBuildingDamage(target, raw);
    clampContext(context);
    return dealt;
}

int Luffy::specialSkill(Building* target, BattleContext& context) {
    if (!target || energy < 20 || hp < ceilMul(maxHp, 0.15)) return 0;
    consumeEnergy(20);
    int raw = ceilMul(atk, 2.0);
    int dealt = dealBuildingDamage(target, raw);
    modifySpeed(15);
    modifyATK(15);
    context.alarmLevel += 10;
    receiveDamage(ceilMul(maxHp, 0.08) + def);
    clampContext(context);
    return dealt;
}

void Luffy::endTurn(BattleContext& context) {
    if (hp <= ceilMul(maxHp, 0.3)) context.morale += 3;
    if (didLastKnockout()) restoreEnergy(5);
    clampContext(context);
}

/*
 * Zoro
 */
Zoro::Zoro(string name, int hp, int atk, int def,
           int speed, int energy, long long bounty)
    : StrawHat(name, hp, atk, def, speed, energy, bounty) {}

int Zoro::attack(Character* target, BattleContext& context) {
    if (!target || !isAlive()) return 0;
    int raw = atk + ceilMul(def, 0.2);
    if (target->getHP() < ceilMul(target->getMaxHP(), 0.4)) raw = ceilMul(raw, 1.15);
    bool wasAlive = target->isAlive();
    int dealt = dealCharacterDamage(target, raw);
    if (wasAlive && !target->isAlive()) context.morale += 5;
    clampContext(context);
    return dealt;
}

int Zoro::specialSkill(Character* target, BattleContext& context) {
    if (!target || energy < 15) return 0;
    consumeEnergy(15);
    int raw = ceilMul(atk, 2.2);
    if (target->getHP() < ceilMul(target->getMaxHP(), 0.5)) raw = ceilMul(atk, 2.7);
    bool wasAlive = target->isAlive();
    int dealt = dealCharacterDamage(target, raw);
    if (wasAlive && !target->isAlive()) {
        restoreEnergy(8);
        context.morale += 4;
    }
    clampContext(context);
    return dealt;
}

int Zoro::attack(Building* target, BattleContext& context) {
    if (!target || !isAlive()) return 0;
    int raw = atk + ceilMul(def, 0.2);
    int dealt = dealBuildingDamage(target, raw);
    clampContext(context);
    return dealt;
}

int Zoro::specialSkill(Building* target, BattleContext& context) {
    if (!target || energy < 15) return 0;
    consumeEnergy(15);
    int raw = ceilMul(atk, 2.2);
    int dealt = dealBuildingDamage(target, raw);
    clampContext(context);
    return dealt;
}

void Zoro::endTurn(BattleContext& context) {
    if (didLastKnockout()) {
        context.morale += 6;
        modifyATK(ceilMul(atk, 0.05));
    }
    clampContext(context);
}

/*
 * Sanji
 */
Sanji::Sanji(string name, int hp, int atk, int def,
             int speed, int energy, long long bounty)
    : StrawHat(name, hp, atk, def, speed, energy, bounty) {}

int Sanji::attack(Character* target, BattleContext& context) {
    if (!target || !isAlive()) return 0;
    int raw = atk + ceilMul(speed, 0.5);
    if (target->getDEF() < def) raw = ceilMul(raw, 1.1);
    bool wasAlive = target->isAlive();
    int dealt = dealCharacterDamage(target, raw);
    if (wasAlive && !target->isAlive()) context.morale += 5;
    clampContext(context);
    return dealt;
}

int Sanji::specialSkill(Character* target, BattleContext& context) {
    if (!target || energy < 18) return 0;
    consumeEnergy(18);
    int raw = ceilMul(atk, 2.1);
    bool wasAlive = target->isAlive();
    int dealt = dealCharacterDamage(target, raw);
    int defDown = (target->getName() == "Jabra") ? -12 : -8;
    target->modifyDEF(defDown);
    if (wasAlive && !target->isAlive()) context.morale += 5;
    clampContext(context);
    return dealt;
}

int Sanji::attack(Building* target, BattleContext& context) {
    if (!target || !isAlive()) return 0;
    int raw = atk + ceilMul(speed, 0.5);
    int dealt = dealBuildingDamage(target, raw);
    clampContext(context);
    return dealt;
}

int Sanji::specialSkill(Building* target, BattleContext& context) {
    if (!target || energy < 18) return 0;
    consumeEnergy(18);
    int raw = ceilMul(atk, 2.1);
    int dealt = dealBuildingDamage(target, raw);
    clampContext(context);
    return dealt;
}

void Sanji::endTurn(BattleContext& context) {
    if (didLastKnockout()) {
        context.morale += 8;
        modifyATK(ceilMul(atk, 0.1));
    }
    clampContext(context);
}

/*
 * Nami
 */
Nami::Nami(string name, int hp, int atk, int def,
           int speed, int energy, long long bounty)
    : StrawHat(name, hp, atk, def, speed, energy, bounty) {}

int Nami::attack(Character* target, BattleContext& context) {
    if (!target || !isAlive()) return 0;
    int effectiveDef = ceilMul(target->getDEF(), 0.7);
    int raw = atk + target->getDEF() - effectiveDef;
    bool wasAlive = target->isAlive();
    int dealt = dealCharacterDamage(target, raw);
    if (wasAlive && !target->isAlive()) context.morale += 5;
    clampContext(context);
    return dealt;
}

int Nami::specialSkill(Character* target, BattleContext& context) {
    if (!target || energy < 20) return 0;
    consumeEnergy(20);
    int raw = atk + 40;
    bool wasAlive = target->isAlive();
    int dealt = dealCharacterDamage(target, raw);
    target->modifySpeed(-10);
    context.busterCallTimer += 1;
    context.alarmLevel -= 5;
    if (wasAlive && !target->isAlive()) context.morale += 5;
    clampContext(context);
    return dealt;
}

int Nami::attack(Building* target, BattleContext& context) {
    if (!target || !isAlive()) return 0;
    int raw = ceilMul(atk, 0.5);
    int dealt = dealBuildingDamage(target, raw);
    clampContext(context);
    return dealt;
}

int Nami::specialSkill(Building* target, BattleContext& context) {
    if (!target || energy < 20) return 0;
    consumeEnergy(20);
    int raw = ceilMul(atk + 40, 1.5);
    int dealt = dealBuildingDamage(target, raw);
    context.busterCallTimer += 1;
    context.alarmLevel -= 5;
    clampContext(context);
    return dealt;
}

void Nami::endTurn(BattleContext& context) {
    if (didLastKnockout()) restoreEnergy(6);
    clampContext(context);
}

/*
 * Chopper
 */
Chopper::Chopper(string name, int hp, int atk, int def,
                 int speed, int energy, long long bounty)
    : StrawHat(name, hp, atk, def, speed, energy, bounty) {}

int Chopper::attack(Character* target, BattleContext& context) {
    if (!target || !isAlive()) return 0;
    int raw = atk;
    bool wasAlive = target->isAlive();
    int dealt = dealCharacterDamage(target, raw);
    if (wasAlive && !target->isAlive()) context.morale += 5;
    clampContext(context);
    return dealt;
}

int Chopper::specialSkill(Character* target, BattleContext& context) {
    if (!target || energy < 15 || !target->isStrawHat()) return 0;
    consumeEnergy(15);
    int healAmount = 35 + ceilMul(atk, 0.5);
    target->heal(healAmount);
    if (target->getName() == "Luffy") context.morale += 5;
    clampContext(context);
    return healAmount;
}

int Chopper::attack(Building* target, BattleContext& context) {
    if (!target || !isAlive()) return 0;
    int raw = atk;
    int dealt = dealBuildingDamage(target, raw);
    clampContext(context);
    return dealt;
}

void Chopper::endTurn(BattleContext& context) {
    clampContext(context);
}

/*
 * Usopp
 */
Usopp::Usopp(string name, int hp, int atk, int def,
             int speed, int energy, long long bounty)
    : StrawHat(name, hp, atk, def, speed, energy, bounty) {}

int Usopp::attack(Character* target, BattleContext& context) {
    if (!target || !isAlive()) return 0;
    int raw = atk;
    if (target->getSpeed() < 50) raw = ceilMul(raw, 1.2);
    bool wasAlive = target->isAlive();
    int dealt = dealCharacterDamage(target, raw);
    if (wasAlive && !target->isAlive()) context.morale += 5;
    clampContext(context);
    return dealt;
}

int Usopp::specialSkill(Character* target, BattleContext& context) {
    if (!target || energy < 16) return 0;
    consumeEnergy(16);
    int raw = ceilMul(atk, 0.8);
    bool wasAlive = target->isAlive();
    int dealt = dealCharacterDamage(target, raw);
    target->modifySpeed(-12);
    context.escapeProgress += 8;
    if (wasAlive && !target->isAlive()) context.morale += 5;
    clampContext(context);
    return dealt;
}

int Usopp::attack(Building* target, BattleContext& context) {
    if (!target || !isAlive()) return 0;
    int raw = ceilMul(atk, 0.5);
    int dealt = dealBuildingDamage(target, raw);
    clampContext(context);
    return dealt;
}

int Usopp::specialSkill(Building* target, BattleContext& context) {
    if (!target || energy < 16) return 0;
    consumeEnergy(16);
    int raw = ceilMul(atk, 0.8);
    int dealt = dealBuildingDamage(target, raw);
    context.escapeProgress += 8;
    clampContext(context);
    return dealt;
}

void Usopp::endTurn(BattleContext& context) {
    context.morale += 10;
    clampContext(context);
}

/*
 * Franky
 */
Franky::Franky(string name, int hp, int atk, int def,
               int speed, int energy, long long bounty)
    : StrawHat(name, hp, atk, def, speed, energy, bounty) {}

int Franky::attack(Character* target, BattleContext& context) {
    if (!target || !isAlive()) return 0;
    int raw = atk + ceilMul(def, 0.3);
    if (target->isCP9()) raw = ceilMul(raw, 1.1);
    bool wasAlive = target->isAlive();
    int dealt = dealCharacterDamage(target, raw);
    if (wasAlive && !target->isAlive()) context.morale += 5;
    clampContext(context);
    return dealt;
}

int Franky::specialSkill(Character* target, BattleContext& context) {
    if (!target) return 0;
    if (energy >= 30) {
        consumeEnergy(30);
        int raw = ceilMul(atk, 1.2);
        bool wasAlive = target->isAlive();
        int dealt = dealCharacterDamage(target, raw);
        if (wasAlive && !target->isAlive()) context.morale += 5;
        clampContext(context);
        return dealt;
    }
    if (energy >= 20) {
        consumeEnergy(20);
        int raw = ceilMul(atk, 1.8);
        if (target->getName() == "Lucci") raw = ceilMul(atk, 2.0);
        bool wasAlive = target->isAlive();
        int dealt = dealCharacterDamage(target, raw);
        target->modifySpeed(-8);
        if (wasAlive && !target->isAlive()) context.morale += 5;
        clampContext(context);
        return dealt;
    }
    return 0;
}

int Franky::attack(Building* target, BattleContext& context) {
    if (!target || !isAlive()) return 0;
    int raw = atk + ceilMul(def, 0.3);
    int dealt = dealBuildingDamage(target, raw);
    clampContext(context);
    return dealt;
}

int Franky::specialSkill(Building* target, BattleContext& context) {
    if (!target) return 0;
    if (energy >= 30) {
        consumeEnergy(30);
        int raw = target->getHP();
        int dealt = dealBuildingDamage(target, raw);
        clampContext(context);
        return dealt;
    }
    if (energy >= 20) {
        consumeEnergy(20);
        int raw = ceilMul(atk, 1.8);
        int dealt = dealBuildingDamage(target, raw);
        clampContext(context);
        return dealt;
    }
    return 0;
}

void Franky::endTurn(BattleContext& context) {
    if (hp > ceilMul(maxHp, 0.7)) modifyDEF(5);
    if (hp < ceilMul(maxHp, 0.3)) modifyATK(ceilMul(atk, 0.1));
    clampContext(context);
}

/*
 * CP9Agent
 */
CP9Agent::CP9Agent() : Character() {
    doriki = 0;
}

CP9Agent::CP9Agent(string name, int hp, int atk, int def,
                   int speed, int energy, int doriki) {
    this->name = name;
    this->hp = hp;
    this->maxHp = hp;
    this->atk = atk;
    this->def = def;
    this->speed = speed;
    this->energy = clampInt(energy, 0, 100);
    this->alive = (hp > 0);
    this->lastKnockout = false;
    if (this->hp < 0) this->hp = 0;
    if (this->maxHp < 0) this->maxHp = 0;
    this->doriki = doriki;
}

bool CP9Agent::isCP9() const {
    return true;
}

string CP9Agent::str() const {
    stringstream ss;
    ss << "CP9[name=" << name
       << ", hp=" << hp
       << ", atk=" << atk
       << ", def=" << def
       << ", speed=" << speed
       << ", energy=" << energy
       << ", doriki=" << doriki << "]";
    return ss.str();
}

/*
 * Lucci
 */
Lucci::Lucci(string name, int hp, int atk, int def,
             int speed, int energy, int doriki)
    : CP9Agent(name, hp, atk, def, speed, energy, doriki) {}

int Lucci::attack(Character* target, BattleContext& context) {
    if (!target || !isAlive()) return 0;
    int raw = atk + doriki / 20;
    if (target->getHP() < ceilMul(target->getMaxHP(), 0.5)) raw = ceilMul(raw, 1.2);
    bool wasAlive = target->isAlive();
    int dealt = dealCharacterDamage(target, raw);
    if (wasAlive && !target->isAlive()) context.morale -= 5;
    clampContext(context);
    return dealt;
}

int Lucci::specialSkill(Character* target, BattleContext& context) {
    if (!target || energy < 25) return 0;
    consumeEnergy(25);
    int effectiveDef = ceilMul(target->getDEF(), 0.5);
    int raw = ceilMul(atk, 2.8) + target->getDEF() - effectiveDef;
    bool wasAlive = target->isAlive();
    int dealt = dealCharacterDamage(target, raw);
    if (wasAlive && !target->isAlive()) context.morale -= 15;
    clampContext(context);
    return dealt;
}

void Lucci::endTurn(BattleContext& context) {
    if (hp < ceilMul(maxHp, 0.4)) modifyATK(ceilMul(atk, 0.05));
    clampContext(context);
}

/*
 * Kaku
 */
Kaku::Kaku(string name, int hp, int atk, int def,
           int speed, int energy, int doriki)
    : CP9Agent(name, hp, atk, def, speed, energy, doriki) {}

int Kaku::attack(Character* target, BattleContext& context) {
    if (!target || !isAlive()) return 0;
    int raw = atk;
    bool wasAlive = target->isAlive();
    int dealt = dealCharacterDamage(target, raw);
    if (wasAlive && !target->isAlive()) context.morale -= 5;
    clampContext(context);
    return dealt;
}

int Kaku::specialSkill(Character* target, BattleContext& context) {
    if (!target || energy < 20) return 0;
    consumeEnergy(20);
    int seq[3] = {ceilMul(atk, 1.2), atk, ceilMul(atk, 0.8)};
    int total = 0;
    for (int i = 0; i < 3; ++i) {
        if (!target->isAlive()) break;
        bool wasAlive = target->isAlive();
        total += dealCharacterDamage(target, seq[i]);
        if (wasAlive && !target->isAlive()) context.morale -= 5;
    }
    clampContext(context);
    return total;
}

void Kaku::endTurn(BattleContext& context) {
    clampContext(context);
}

/*
 * Jabra
 */
Jabra::Jabra(string name, int hp, int atk, int def,
             int speed, int energy, int doriki)
    : CP9Agent(name, hp, atk, def, speed, energy, doriki) {}

int Jabra::attack(Character* target, BattleContext& context) {
    if (!target || !isAlive()) return 0;
    int raw = atk;
    bool wasAlive = target->isAlive();
    int dealt = dealCharacterDamage(target, raw);
    if (wasAlive && !target->isAlive()) context.morale -= 5;
    clampContext(context);
    return dealt;
}

int Jabra::specialSkill(Character* target, BattleContext& context) {
    if (!target || energy < 18) return 0;
    consumeEnergy(18);
    int raw = ceilMul(atk, 1.5);
    if (hp < ceilMul(maxHp, 0.3)) raw = ceilMul(atk, 1.75);
    bool wasAlive = target->isAlive();
    int dealt = dealCharacterDamage(target, raw);
    if (wasAlive && !target->isAlive()) context.morale -= 10;
    clampContext(context);
    return dealt;
}

void Jabra::endTurn(BattleContext& context) {
    clampContext(context);
}

/*
 * Blueno
 */
Blueno::Blueno(string name, int hp, int atk, int def,
               int speed, int energy, int doriki)
    : CP9Agent(name, hp, atk, def, speed, energy, doriki) {}

int Blueno::attack(Character* target, BattleContext& context) {
    if (!target || !isAlive()) return 0;
    int raw = atk;
    bool wasAlive = target->isAlive();
    int dealt = dealCharacterDamage(target, raw);
    if (wasAlive && !target->isAlive()) context.morale -= 5;
    clampContext(context);
    return dealt;
}

int Blueno::specialSkill(Character* target, BattleContext& context) {
    if (!target || energy < 15) return 0;
    consumeEnergy(15);
    int raw = ceilMul(atk, 1.3);
    raw += (hp > ceilMul(maxHp, 0.5)) ? 20 : 40;
    bool wasAlive = target->isAlive();
    int dealt = dealCharacterDamage(target, raw);
    if (wasAlive && !target->isAlive()) context.morale -= 5;
    clampContext(context);
    return dealt;
}

void Blueno::endTurn(BattleContext& context) {
    clampContext(context);
}

/*
 * Kalifa
 */
Kalifa::Kalifa(string name, int hp, int atk, int def,
               int speed, int energy, int doriki)
    : CP9Agent(name, hp, atk, def, speed, energy, doriki) {}

int Kalifa::attack(Character* target, BattleContext& context) {
    if (!target || !isAlive()) return 0;
    int raw = atk;
    bool wasAlive = target->isAlive();
    int dealt = dealCharacterDamage(target, raw);
    if (wasAlive && !target->isAlive()) context.morale -= 5;
    clampContext(context);
    return dealt;
}

int Kalifa::specialSkill(Character* target, BattleContext& context) {
    if (!target || energy < 18) return 0;
    consumeEnergy(18);
    int raw = ceilMul(atk, 1.4);
    bool wasAlive = target->isAlive();
    int dealt = dealCharacterDamage(target, raw);
    context.morale -= (target->getName() == "Nami") ? 12 : 8;
    target->modifySpeed(-6);
    if (wasAlive && !target->isAlive()) context.morale -= 5;
    clampContext(context);
    return dealt;
}

void Kalifa::endTurn(BattleContext& context) {
    clampContext(context);
}

/*
 * Kumadori
 */
Kumadori::Kumadori(string name, int hp, int atk, int def,
                   int speed, int energy, int doriki)
    : CP9Agent(name, hp, atk, def, speed, energy, doriki) {}

int Kumadori::attack(Character* target, BattleContext& context) {
    if (!target || !isAlive()) return 0;
    int raw = atk;
    bool wasAlive = target->isAlive();
    int dealt = dealCharacterDamage(target, raw);
    if (wasAlive && !target->isAlive()) context.morale -= 5;
    clampContext(context);
    return dealt;
}

int Kumadori::specialSkill(Character* target, BattleContext& context) {
    if (!target || energy < 16) return 0;
    consumeEnergy(16);
    int raw = 30 + doriki / 10;
    if (hp < ceilMul(maxHp, 0.4)) raw += 25;
    bool wasAlive = target->isAlive();
    int dealt = dealCharacterDamage(target, raw);
    if (wasAlive && !target->isAlive()) context.morale -= 5;
    clampContext(context);
    return dealt;
}

void Kumadori::endTurn(BattleContext& context) {
    clampContext(context);
}

/*
 * Fukurou
 */
Fukurou::Fukurou(string name, int hp, int atk, int def,
                 int speed, int energy, int doriki)
    : CP9Agent(name, hp, atk, def, speed, energy, doriki) {}

int Fukurou::attack(Character* target, BattleContext& context) {
    if (!target || !isAlive()) return 0;
    int raw = atk;
    bool wasAlive = target->isAlive();
    int dealt = dealCharacterDamage(target, raw);
    if (wasAlive && !target->isAlive()) context.morale -= 5;
    clampContext(context);
    return dealt;
}

int Fukurou::specialSkill(Character* target, BattleContext& context) {
    if (!target || energy < 14) return 0;
    consumeEnergy(14);
    int raw = ceilMul(atk, 1.3);
    
    if (context.strawHats && context.strawHatCount > 0 && target->isStrawHat()) {
        int minHp = INT_MAX;
        for (int i = 0; i < context.strawHatCount; ++i) {
            if (context.strawHats[i]->isAlive() && context.strawHats[i]->getHP() < minHp) {
                minHp = context.strawHats[i]->getHP();
            }
        }
        if (target->getHP() == minHp) {
            raw += 20;
        }
    }
    
    bool wasAlive = target->isAlive();
    int dealt = dealCharacterDamage(target, raw);
    if (wasAlive && !target->isAlive()) context.morale -= 11;
    clampContext(context);
    return dealt;
}

void Fukurou::endTurn(BattleContext& context) {
    clampContext(context);
}

/*
 * Building
 */
Building::Building(string name, int hp) {
    this->name = name;
    this->hp = hp;
    this->maxHP = hp;
    this->destroyed = (hp <= 0);
    if (this->hp < 0) this->hp = 0;
    if (this->maxHP < 0) this->maxHP = 0;
}

Building::~Building() {
    return;
}

void Building::receiveDamage(int damage) {
    if (destroyed) return;
    hp -= damage;
    if (hp <= 0) {
        hp = 0;
        destroyed = true;
    }
}

bool Building::isDestroyed() const {
    return destroyed;
}

string Building::getName() const { return name; }
int Building::getHP() const { return hp; }
int Building::getMaxHP() const { return maxHP; }

void Building::onDestroyed(BattleContext& context) {
    return ;
}

string Building::str () const {
    stringstream ss;
    ss << "Building[name=" << name
       << ", hp=" << hp
       << ", maxHP=" << maxHP
       << ", destroyed=" << (destroyed ? "true" : "false") << "]";
    return ss.str();
}

/*
 * MainGate
 */
MainGate::MainGate(string name, int hp) : Building(name, hp) {}

void MainGate::applyEffect(BattleContext& context) {
    if (!destroyed) return;
    clampContext(context);
}

void MainGate::onDestroyed(BattleContext& context) {
    context.mainGateDestroyed = true;
    context.rescueProgress += 20;
    context.morale += 5;
    clampContext(context);
}

/*
 * Courthouse
 */
Courthouse::Courthouse(string name, int hp) : Building(name, hp) {}

void Courthouse::applyEffect(BattleContext& context) {
    if (!destroyed) context.alarmLevel += 5;
    clampContext(context);
}

void Courthouse::onDestroyed(BattleContext& context) {
    context.alarmLevel -= 20;
    clampContext(context);
}

/*
 * TowerOfJustice
 */
TowerOfJustice::TowerOfJustice(string name, int hp) : Building(name, hp) {}

void TowerOfJustice::applyEffect(BattleContext& context) {
    if (destroyed) return;
    if (context.mainGateDestroyed && !context.robinRescued) context.rescueProgress += 5;
    if (context.rescueProgress >= 100 && !context.robinRescued) {
        context.robinRescued = true;
        context.morale += 10;
    }
    clampContext(context);
}

/*
 * BridgeOfHesitation
 */
BridgeOfHesitation::BridgeOfHesitation(string name, int hp) : Building(name, hp) {}

void BridgeOfHesitation::applyEffect(BattleContext& context) {
    if (destroyed) return;
    if (context.robinRescued) {
        context.bridgeOpened = true;
        context.escapeProgress += 5;
        if (context.escapeProgress >= 100) {
            context.battleEnded = true;
            context.resultCode = "STRAW_HAT_WIN";
        }
    }
    clampContext(context);
}

/*
 * BusterCallShip
 */
BusterCallShip::BusterCallShip(string name, int hp) : Building(name, hp) {}

void BusterCallShip::applyEffect(BattleContext& context) {
    if (destroyed) return;
    context.busterCallTimer--;
    if (context.busterCallTimer <= 0) {
        context.battleEnded = true;
        context.resultCode = "BUSTER_CALL";
    }
    clampContext(context);
}

void BusterCallShip::onDestroyed(BattleContext& context) {
    context.busterCallTimer += 3;
    clampContext(context);
}

/*
 * EniesLobbyBattle
 */
EniesLobbyBattle::EniesLobbyBattle(const string& filename) {
    strawHats = new Character*[7];
    cp9Agents = new Character*[7];
    buildings = new Building*[5];
    strawHatCount = 0;
    cp9Count = 0;
    buildingCount = 0;
    turnOrder = NULL;
    maxTurns = 0;
    loadFromFile(filename);
    context.strawHats = strawHats;
    context.strawHatCount = strawHatCount;
}

EniesLobbyBattle::~EniesLobbyBattle() {
    for (int i = 0; i < strawHatCount; ++i) delete strawHats[i];
    for (int i = 0; i < cp9Count; ++i) delete cp9Agents[i];
    for (int i = 0; i < buildingCount; ++i) delete buildings[i];
    delete[] strawHats;
    delete[] cp9Agents;
    delete[] buildings;
    while (turnOrder) {
        TurnNode* tmp = turnOrder;
        turnOrder = turnOrder->next;
        delete tmp;
    }
}

void EniesLobbyBattle::loadFromFile(const string& filename) {
    ifstream fin(filename.c_str());
    string type;
    while (fin >> type) {
        if (type == "CONTEXT") {
            fin >> context.morale >> context.alarmLevel >> context.rescueProgress
                >> context.escapeProgress >> context.busterCallTimer >> maxTurns;
            clampContext(context);
        } else if (type == "STRAW_HAT") {
            string n; int hp, atk, def, speed, energy; long long bounty;
            fin >> n >> hp >> atk >> def >> speed >> energy >> bounty;
            Character* c = NULL;
            if (n == "Luffy") c = new Luffy(n, hp, atk, def, speed, energy, bounty);
            else if (n == "Zoro") c = new Zoro(n, hp, atk, def, speed, energy, bounty);
            else if (n == "Sanji") c = new Sanji(n, hp, atk, def, speed, energy, bounty);
            else if (n == "Nami") c = new Nami(n, hp, atk, def, speed, energy, bounty);
            else if (n == "Chopper") c = new Chopper(n, hp, atk, def, speed, energy, bounty);
            else if (n == "Usopp" || n == "Sogeking") c = new Usopp(n, hp, atk, def, speed, energy, bounty);
            else if (n == "Franky") c = new Franky(n, hp, atk, def, speed, energy, bounty);
            if (c) addStrawHat(c);
        } else if (type == "CP9") {
            string n; int hp, atk, def, speed, energy, doriki;
            fin >> n >> hp >> atk >> def >> speed >> energy >> doriki;
            Character* c = NULL;
            if (n == "Lucci") c = new Lucci(n, hp, atk, def, speed, energy, doriki);
            else if (n == "Kaku") c = new Kaku(n, hp, atk, def, speed, energy, doriki);
            else if (n == "Jabra") c = new Jabra(n, hp, atk, def, speed, energy, doriki);
            else if (n == "Blueno") c = new Blueno(n, hp, atk, def, speed, energy, doriki);
            else if (n == "Kalifa") c = new Kalifa(n, hp, atk, def, speed, energy, doriki);
            else if (n == "Kumadori") c = new Kumadori(n, hp, atk, def, speed, energy, doriki);
            else if (n == "Fukurou") c = new Fukurou(n, hp, atk, def, speed, energy, doriki);
            if (c) addCP9Agent(c);
        } else if (type == "BUILDING") {
            string n; int hp;
            fin >> n >> hp;
            Building* b = NULL;
            if (n == "MainGate") b = new MainGate(n, hp);
            else if (n == "Courthouse") b = new Courthouse(n, hp);
            else if (n == "TowerOfJustice") b = new TowerOfJustice(n, hp);
            else if (n == "BridgeOfHesitation") b = new BridgeOfHesitation(n, hp);
            else if (n == "BusterCallShip") b = new BusterCallShip(n, hp);
            if (b) addBuilding(b);
        }
    }
    buildTurnOrder();
}

void EniesLobbyBattle::addStrawHat(Character* character) {
    if (strawHatCount < 7) strawHats[strawHatCount++] = character;
}

void EniesLobbyBattle::addCP9Agent(Character* character) {
    if (cp9Count < 7) cp9Agents[cp9Count++] = character;
}

void EniesLobbyBattle::addBuilding(Building* building) {
    if (buildingCount < 5) buildings[buildingCount++] = building;
}

void EniesLobbyBattle::buildTurnOrder() {
    while (turnOrder) {
        TurnNode* tmp = turnOrder;
        turnOrder = turnOrder->next;
        delete tmp;
    }
    TurnNode* tail = NULL;
    for (int i = 0; i < strawHatCount; ++i) {
        TurnNode* node = new TurnNode;
        node->data = strawHats[i];
        node->next = NULL;
        if (!turnOrder) turnOrder = node;
        else tail->next = node;
        tail = node;
    }
    for (int i = 0; i < cp9Count; ++i) {
        TurnNode* node = new TurnNode;
        node->data = cp9Agents[i];
        node->next = NULL;
        if (!turnOrder) turnOrder = node;
        else tail->next = node;
        tail = node;
    }
}

void EniesLobbyBattle::runBattle() {
    while (!context.battleEnded && context.turnCount < maxTurns && turnOrder) {
        Character* actor = turnOrder->data;
        processTurn(actor);
        if (turnOrder && turnOrder->next) {
            TurnNode* first = turnOrder;
            turnOrder = turnOrder->next;
            first->next = NULL;
            TurnNode* tail = turnOrder;
            while (tail->next) tail = tail->next;
            tail->next = first;
        }
        processBuildings();
        context.nextTurn();
        checkEndCondition();
    }
    if (!context.battleEnded && context.turnCount >= maxTurns) {
        context.battleEnded = true;
        context.resultCode = "TIME_OUT";
    }
}

void EniesLobbyBattle::processTurn(Character* character) {
    if (!character || !character->isAlive()) return;
    character->setLastKnockout(false);

    Building* mainGate = NULL;
    Building* courthouse = NULL;
    Building* bridge = NULL;
    Building* busterShip = NULL;
    for (int i = 0; i < buildingCount; ++i) {
        if (buildings[i]->getName() == "MainGate") mainGate = buildings[i];
        else if (buildings[i]->getName() == "Courthouse") courthouse = buildings[i];
        else if (buildings[i]->getName() == "BridgeOfHesitation") bridge = buildings[i];
        else if (buildings[i]->getName() == "BusterCallShip") busterShip = buildings[i];
    }

    Character* targetChar = NULL;
    Building* targetBuilding = NULL;
    if (character->isStrawHat()) {
        if (character->getName() == "Chopper" && character->getEnergy() >= 15) {
            for (int i = 0; i < strawHatCount; ++i) {
                if (!strawHats[i]->isAlive()) continue;
                if (!targetChar || strawHats[i]->getHP() < targetChar->getHP()) targetChar = strawHats[i];
            }
        } else if (mainGate && !mainGate->isDestroyed()) targetBuilding = mainGate;
        else if (context.alarmLevel >= 50 && courthouse && !courthouse->isDestroyed()) targetBuilding = courthouse;
        else if (context.busterCallTimer <= 5 && busterShip && !busterShip->isDestroyed()) targetBuilding = busterShip;
        else if (!context.robinRescued) {
            for (int i = 0; i < cp9Count; ++i) if (cp9Agents[i]->isAlive()) { targetChar = cp9Agents[i]; break; }
        } else {
            if (bridge && !bridge->isDestroyed()) targetBuilding = bridge;
            else for (int i = 0; i < cp9Count; ++i) if (cp9Agents[i]->isAlive()) { targetChar = cp9Agents[i]; break; }
        }
    } else {
        for (int i = 0; i < strawHatCount; ++i) if (strawHats[i]->isAlive()) { targetChar = strawHats[i]; break; }
    }

    int dmg = 0;
    bool usedSkill = false;
    if (targetBuilding) {
        bool before = targetBuilding->isDestroyed();
        int energyBefore = character->getEnergy();
        dmg = character->specialSkill(targetBuilding, context);
        usedSkill = (character->getEnergy() < energyBefore);
        if (!usedSkill) dmg = character->attack(targetBuilding, context);
        if (!before && targetBuilding->isDestroyed()) targetBuilding->onDestroyed(context);
    } else if (targetChar) {
        bool aliveBefore = targetChar->isAlive();
        int energyBefore = character->getEnergy();
        dmg = character->specialSkill(targetChar, context);
        usedSkill = (character->getEnergy() < energyBefore);
        if (!usedSkill && targetChar->isAlive()) dmg = character->attack(targetChar, context);
        if (aliveBefore && !targetChar->isAlive()) character->setLastKnockout(true);
    }
    character->endTurn(context);
    clampContext(context);
}

void EniesLobbyBattle::processBuildings() {
    for (int i = 0; i < buildingCount; ++i) buildings[i]->applyEffect(context);
    clampContext(context);
}

void EniesLobbyBattle::checkEndCondition() {
    if (context.robinRescued && context.escapeProgress >= 100) {
        context.battleEnded = true;
        context.resultCode = "STRAW_HAT_WIN";
        return;
    }
    if (context.busterCallTimer <= 0) {
        context.battleEnded = true;
        context.resultCode = "BUSTER_CALL";
        return;
    }
    bool allSHDown = true;
    for (int i = 0; i < strawHatCount; ++i) if (strawHats[i]->isAlive()) { allSHDown = false; break; }
    if (allSHDown) {
        context.battleEnded = true;
        context.resultCode = "CP9_WIN";
        return;
    }
    bool allCP9Down = true;
    for (int i = 0; i < cp9Count; ++i) if (cp9Agents[i]->isAlive()) { allCP9Down = false; break; }
    if (allCP9Down) {
        context.battleEnded = true;
        context.resultCode = "STRAW_HAT_WIN_BY_DEFEAT_CP9";
        return;
    }
    if (context.turnCount >= maxTurns) {
        context.battleEnded = true;
        context.resultCode = "TIME_OUT";
    }
}

string EniesLobbyBattle::getResult() const {
    stringstream ss;
    ss << context.resultCode << " "
       << context.turnCount << " "
       << context.morale << " "
       << context.alarmLevel << " "
       << context.rescueProgress << " "
       << context.escapeProgress << " "
       << context.busterCallTimer;
    return ss.str();
}