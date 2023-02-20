// SPDX-License-Identifier: GPL-2.0-only
/*
 * Driver for GP7101 Generic Pulse Width Modulator
 *
 * Copyright (C) 2023
 * Author: Yatao Li <yatao.li@live.com>
 */

#include <linux/module.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/i2c.h>
#include <linux/pwm.h>
#include <linux/regmap.h>

/*
 * This driver handles the PWM of GP7101.
 */

struct gp7101_pwm_chip {
	struct pwm_chip chip;
	struct regmap *regmap;
};

static inline struct gp7101_pwm_chip *to_gp7101(struct pwm_chip *chip)
{
	return container_of(chip, struct gp7101_pwm_chip, chip);
}

static int gp7101_write_reg(struct gp7101_pwm_chip *gp7101, unsigned int reg, unsigned int val)
{
	struct device *dev = gp7101->chip.dev;
	int err;

	err = regmap_write(gp7101->regmap, reg, val);
	if (err)
		dev_err(dev, "regmap_write to register 0x%x failed: %pe\n", reg, ERR_PTR(err));

	return err;
}

static int gp7101_pwm_apply(struct pwm_chip *chip, struct pwm_device *pwm,
			     const struct pwm_state *state)
{
  /* gp7101 is fixed frequency so we ignore the period setting here */
  struct gp7101_pwm_chip* gp7101 = to_gp7101(chip);
  unsigned long long duty = 0;

	if (state->polarity != PWM_POLARITY_NORMAL)
		return -EINVAL;

	if (state->enabled) {
    duty = DIV_ROUND_DOWN_ULL(state->duty_cycle * 0xFF, state->period);
	}

	return gp7101_write_reg(gp7101, 0x03, duty & 0xFF);
}

static const struct pwm_ops gp7101_pwm_ops = {
	.apply = gp7101_pwm_apply,
	.owner = THIS_MODULE,
};

static const struct regmap_config gp7101_regmap_i2c_config = {
	.reg_bits = 8,
	.val_bits = 8,
	.max_register = 0x3,
	.cache_type = REGCACHE_NONE,
};


static int gp7101_pwm_probe(struct i2c_client *client, 
    const struct i2c_device_id *id)
{
	struct gp7101_pwm_chip *gp7101;
  int ret;

	gp7101 = devm_kzalloc(&client->dev, sizeof(*gp7101), GFP_KERNEL);
	if (!gp7101)
		return -ENOMEM;

  gp7101->regmap = devm_regmap_init_i2c(client, &gp7101_regmap_i2c_config);
	if (IS_ERR(gp7101->regmap)) {
		ret = PTR_ERR(gp7101->regmap);
		dev_err(&client->dev, "Failed to initialize register map: %d\n",
			ret);
		return ret;
	}

	i2c_set_clientdata(client, gp7101);

  gp7101->chip.ops = &gp7101_pwm_ops;
	gp7101->chip.dev = &client->dev;
	gp7101->chip.npwm = 1;

	return pwmchip_add(&gp7101->chip);
}

#ifdef CONFIG_OF
static const struct of_device_id gp7101_pwm_of_match[] = {
	{ .compatible = "guestgood,gp7101-pwm" },
	{ },
};
MODULE_DEVICE_TABLE(of, gp7101_pwm_of_match);
#endif

static struct i2c_driver gp7101_i2c_driver = {
	.driver = {
		.name = "gp7101-pwm",
		.of_match_table = of_match_ptr(gp7101_pwm_of_match),
	},
	.probe = gp7101_pwm_probe,
};
module_i2c_driver(gp7101_i2c_driver);

MODULE_AUTHOR("Yatao Li <yatao.li@live.com>");
MODULE_DESCRIPTION("PWM driver for GP7101");
MODULE_ALIAS("platform:gp7101-pwm");
MODULE_LICENSE("GPL");
